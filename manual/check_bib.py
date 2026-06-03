#!/usr/bin/env python3
"""
check_bib.py - Validate BibTeX entries against CrossRef API using DOI.

Usage:
    python check_bib.py refs.bib
    python check_bib.py refs.bib --output report.json
    python check_bib.py refs.bib --rate-limit 0.5

Exit codes:
    0 - all DOI entries OK
    1 - mismatch or errors found
    2 - argument / file error
"""

import sys
import json
import time
import re
import argparse
import unicodedata
from difflib import SequenceMatcher
import urllib.request
import urllib.error
from typing import Optional, List

try:
    import bibtexparser
    from bibtexparser.bparser import BibTexParser
    from bibtexparser.customization import convert_to_unicode
except ImportError:
    print("ERROR: bibtexparser not found. Run: pip install bibtexparser", file=sys.stderr)
    sys.exit(2)

# ──────────────────────────────────────────────────────────────────────────────
# Text normalisation for fuzzy comparison
# ──────────────────────────────────────────────────────────────────────────────

def normalise(text: str) -> str:
    """Lowercase, strip accents, collapse whitespace, remove punctuation."""
    text = unicodedata.normalize("NFKD", text)
    text = "".join(c for c in text if not unicodedata.combining(c))
    text = text.lower()
    text = re.sub(r"[^\w\s]", " ", text)
    text = re.sub(r"\s+", " ", text).strip()
    return text

def similarity(a: str, b: str) -> float:
    return SequenceMatcher(None, normalise(a), normalise(b)).ratio()

# ──────────────────────────────────────────────────────────────────────────────
# CrossRef / arXiv API
# ──────────────────────────────────────────────────────────────────────────────

CROSSREF_URL = "https://api.crossref.org/works/{doi}"
USER_AGENT   = "BibTeXChecker/1.0 (mailto:user@example.com)"

# arXiv DOI prefix: 10.48550/arXiv.XXXX.XXXXX
_ARXIV_DOI_RE = re.compile(r"10\.48550/arXiv\.(\d{4}\.\d+)", re.IGNORECASE)

def _is_arxiv_doi(doi: str) -> bool:
    return bool(_ARXIV_DOI_RE.match(doi.strip()))

def _arxiv_id_from_doi(doi: str) -> str:
    m = _ARXIV_DOI_RE.match(doi.strip())
    return m.group(1) if m else ""

def fetch_crossref(doi: str) -> Optional[dict]:
    """Return CrossRef work metadata dict, or None if 404."""
    url = CROSSREF_URL.format(doi=urllib.parse.quote(doi, safe=":/"))
    req = urllib.request.Request(url, headers={"User-Agent": USER_AGENT})
    try:
        with urllib.request.urlopen(req, timeout=15) as resp:
            data = json.loads(resp.read().decode())
            return data.get("message", {})
    except urllib.error.HTTPError as e:
        if e.code == 404:
            return None
        raise
    except Exception:
        raise

# arXiv asks for >= 3s between requests; we add retry on 429 / timeout
ARXIV_TIMEOUT    = 30   # seconds; arXiv can be slow
ARXIV_MAX_RETRY  = 4
ARXIV_RETRY_BASE = 5.0  # seconds; doubled each retry

def fetch_arxiv(doi: str) -> Optional[dict]:
    """Fetch arXiv metadata via Atom feed API with retry on 429 / timeout.

    Returns a normalised dict with keys: title, author (list), year, _source='arxiv'
    Returns None if the arXiv ID is not found (empty feed).
    """
    import xml.etree.ElementTree as ET
    import socket

    arxiv_id = _arxiv_id_from_doi(doi)
    if not arxiv_id:
        return None

    feed_url = f"https://export.arxiv.org/api/query?id_list={arxiv_id}&max_results=1"
    req = urllib.request.Request(feed_url, headers={"User-Agent": USER_AGENT})

    xml_bytes = None
    for attempt in range(ARXIV_MAX_RETRY):
        try:
            with urllib.request.urlopen(req, timeout=ARXIV_TIMEOUT) as resp:
                xml_bytes = resp.read()
            break  # success
        except urllib.error.HTTPError as e:
            if e.code == 404:
                return None
            if e.code == 429:
                wait = ARXIV_RETRY_BASE * (2 ** attempt)
                print(f"    [arXiv 429] rate-limited, waiting {wait:.0f}s before retry {attempt+1}/{ARXIV_MAX_RETRY}...", flush=True)
                time.sleep(wait)
                continue
            raise
        except (urllib.error.URLError, socket.timeout, OSError) as e:
            wait = ARXIV_RETRY_BASE * (2 ** attempt)
            print(f"    [arXiv timeout/err] {e}, waiting {wait:.0f}s before retry {attempt+1}/{ARXIV_MAX_RETRY}...", flush=True)
            time.sleep(wait)
            continue
    else:
        raise RuntimeError(f"arXiv fetch failed after {ARXIV_MAX_RETRY} retries for {arxiv_id}")

    ns = {
        "atom":  "http://www.w3.org/2005/Atom",
        "arxiv": "http://arxiv.org/schemas/atom",
    }
    root = ET.fromstring(xml_bytes)
    entries = root.findall("atom:entry", ns)
    if not entries:
        return None  # ID not found -> empty feed

    entry = entries[0]

    # title
    title_el = entry.find("atom:title", ns)
    title = title_el.text.strip().replace("\n", " ") if title_el is not None else ""

    # authors: arXiv gives "Firstname Lastname" -> convert to "Lastname, Firstname"
    authors = []
    for author_el in entry.findall("atom:author", ns):
        name_el = author_el.find("atom:name", ns)
        if name_el is not None:
            parts = name_el.text.strip().rsplit(" ", 1)
            if len(parts) == 2:
                authors.append(f"{parts[1]}, {parts[0]}")
            else:
                authors.append(parts[0])

    # year: use published date (ISO 8601: "2024-11-29T...")
    pub_el = entry.find("atom:published", ns)
    year = pub_el.text[:4] if pub_el is not None else ""

    return {
        "title":   title,
        "author":  authors,
        "year":    year,
        "_source": "arxiv",
    }

def fetch_metadata(doi: str) -> Optional[dict]:
    """Dispatch to CrossRef or arXiv depending on DOI prefix."""
    if _is_arxiv_doi(doi):
        return fetch_arxiv(doi)
    return fetch_crossref(doi)

# ──────────────────────────────────────────────────────────────────────────────
# Field extractors — unified for CrossRef and arXiv dicts
# ──────────────────────────────────────────────────────────────────────────────

def meta_title(msg: dict) -> str:
    # arXiv: msg["title"] is a plain string
    # CrossRef: msg["title"] is a list
    t = msg.get("title", "")
    if isinstance(t, list):
        return t[0] if t else ""
    return t

def meta_year(msg: dict) -> str:
    # arXiv: msg["year"] is a plain string
    if "_source" in msg:
        return msg.get("year", "")
    # CrossRef: nested date-parts
    for key in ("published-print", "published-online", "issued"):
        dp = msg.get(key, {}).get("date-parts", [[]])
        if dp and dp[0]:
            return str(dp[0][0])
    return ""

def meta_authors(msg: dict) -> List[str]:
    """Return list of 'Lastname, Firstname' strings."""
    # arXiv: msg["author"] is already List[str]
    if "_source" in msg:
        return msg.get("author", [])
    # CrossRef: list of dicts with 'family'/'given'
    authors = []
    for a in msg.get("author", []):
        family = a.get("family", "")
        given  = a.get("given", "")
        authors.append(f"{family}, {given}".strip(", "))
    return authors

def meta_source_label(msg: dict) -> str:
    return msg.get("_source", "crossref")

# ──────────────────────────────────────────────────────────────────────────────
# BibTeX field extractors
# ──────────────────────────────────────────────────────────────────────────────

def bib_authors(entry: dict) -> List[str]:
    raw = entry.get("author", "")
    if not raw:
        return []
    # Split on " and " (case-insensitive)
    parts = re.split(r"\s+and\s+", raw, flags=re.IGNORECASE)
    return [p.strip() for p in parts if p.strip()]

# ──────────────────────────────────────────────────────────────────────────────
# Comparison logic
# ──────────────────────────────────────────────────────────────────────────────

TITLE_THRESHOLD  = 0.85
AUTHOR_THRESHOLD = 0.80

def compare_entry(entry: dict, msg: dict, title_threshold: float = TITLE_THRESHOLD) -> List[dict]:
    """Return list of mismatch dicts for this entry."""
    issues = []

    # Title
    bib_t = entry.get("title", "")
    cr_t  = meta_title(msg)
    if bib_t and cr_t:
        score = similarity(bib_t, cr_t)
        if score < title_threshold:
            issues.append({
                "field": "title",
                "bib":   bib_t,
                "api":   cr_t,
                "similarity": round(score, 3),
            })

    # Year
    bib_y = entry.get("year", "").strip()
    cr_y  = meta_year(msg)
    if bib_y and cr_y and bib_y != cr_y:
        issues.append({
            "field": "year",
            "bib":   bib_y,
            "api":   cr_y,
        })

    # First author (last name only, lenient check)
    bib_auths = bib_authors(entry)
    cr_auths  = meta_authors(msg)
    if bib_auths and cr_auths:
        bib_first_family = normalise(bib_auths[0].split(",")[0])
        cr_first_family  = normalise(cr_auths[0].split(",")[0])
        if bib_first_family and cr_first_family:
            score = similarity(bib_first_family, cr_first_family)
            if score < AUTHOR_THRESHOLD:
                issues.append({
                    "field": "first_author",
                    "bib":   bib_auths[0],
                    "api":   cr_auths[0],
                    "similarity": round(score, 3),
                })

    # Author count sanity check
    if bib_auths and cr_auths:
        if abs(len(bib_auths) - len(cr_auths)) >= 2:
            issues.append({
                "field": "author_count",
                "bib":   len(bib_auths),
                "api":   len(cr_auths),
            })

    return issues

# ──────────────────────────────────────────────────────────────────────────────
# Main
# ──────────────────────────────────────────────────────────────────────────────

import urllib.parse

def main():
    parser = argparse.ArgumentParser(description="Validate BibTeX entries via CrossRef DOI.")
    parser.add_argument("bibfile", help="Input .bib file")
    parser.add_argument("--output", "-o", default=None, help="Write JSON report to this file")
    parser.add_argument("--rate-limit", type=float, default=1.0,
                        help="Seconds between CrossRef API calls (default: 1.0)")
    parser.add_argument("--arxiv-rate-limit", type=float, default=3.0,
                        help="Seconds between arXiv API calls (default: 3.0)")
    parser.add_argument("--threshold-title", type=float, default=TITLE_THRESHOLD,
                        help=f"Title similarity threshold (default: {TITLE_THRESHOLD})")
    args = parser.parse_args()

    title_threshold = args.threshold_title

    # Load .bib
    try:
        with open(args.bibfile, encoding="utf-8") as f:
            raw = f.read()
    except FileNotFoundError:
        print(f"ERROR: file not found: {args.bibfile}", file=sys.stderr)
        sys.exit(2)

    bib_parser = BibTexParser(common_strings=True)
    bib_parser.customization = convert_to_unicode
    db = bibtexparser.loads(raw, parser=bib_parser)
    entries = db.entries

    print(f"Loaded {len(entries)} entries from {args.bibfile}")

    results = {
        "ok":      [],
        "missing_doi": [],
        "doi_not_found": [],
        "mismatch": [],
        "errors":  [],
    }

    for entry in entries:
        key = entry.get("ID", "?")
        doi = entry.get("doi", "").strip()

        if not doi:
            results["missing_doi"].append({"key": key})
            print(f"  [WARN] {key}: no DOI")
            continue

        try:
            wait = args.arxiv_rate_limit if _is_arxiv_doi(doi) else args.rate_limit
            time.sleep(wait)
            msg = fetch_metadata(doi)
        except Exception as e:
            results["errors"].append({"key": key, "doi": doi, "error": str(e)})
            print(f"  [ERROR] {key}: API error - {e}")
            continue

        if msg is None:
            results["doi_not_found"].append({"key": key, "doi": doi})
            print(f"  [ERROR] {key}: DOI not found ({doi})")
            continue

        issues = compare_entry(entry, msg, title_threshold=title_threshold)
        if issues:
            results["mismatch"].append({"key": key, "doi": doi, "issues": issues})
            print(f"  [MISMATCH] {key}:")
            src_label = meta_source_label(msg)
            for iss in issues:
                api_val = iss.get("api", "")
                if "similarity" in iss:
                    print(f"    {iss['field']}: bib={iss['bib']!r}  {src_label}={api_val!r}  sim={iss['similarity']}")
                else:
                    print(f"    {iss['field']}: bib={iss['bib']!r}  {src_label}={api_val!r}")
        else:
            results["ok"].append({"key": key, "doi": doi})
            print(f"  [OK]    {key}")

    # Summary
    print("\n── Summary ──")
    print(f"  OK:             {len(results['ok'])}")
    print(f"  Missing DOI:    {len(results['missing_doi'])}")
    print(f"  DOI not found:  {len(results['doi_not_found'])}")
    print(f"  Mismatch:       {len(results['mismatch'])}")
    print(f"  API errors:     {len(results['errors'])}")

    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            json.dump(results, f, ensure_ascii=False, indent=2)
        print(f"\nReport written to {args.output}")

    n_problems = len(results["doi_not_found"]) + len(results["mismatch"]) + len(results["errors"])
    sys.exit(1 if n_problems else 0)

if __name__ == "__main__":
    main()
