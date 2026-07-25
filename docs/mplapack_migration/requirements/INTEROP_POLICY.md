# gmpfrxx_mkII の MPFR/MPC interop 方針

この文書は、MPLAPACK 周辺で使う外部実数・複素数クラスと
`gmpfrxx_mkII` の `mpfrxx::mpfr_class` / `mpfrxx::mpc_class` を接続する
差し込み式 adapter の方針をまとめるものです。

基本方針は非対称です。

- DD/QD/binary80/binary128/`gmpxx::mpf_class`/`double` から
  `mpfrxx::mpfr_class` / `mpfrxx::mpc_class` へは入れられる。
- 逆方向、つまり `mpfr_class` / `mpc_class` から外部型へ戻す場合は、
  `cast2dd_real()` / `cast2dd_complex()` のような明示関数だけで行う。
- mixed arithmetic operator は adapter の責務にしない。

これにより、暗黙変換による精度低下や意図しない overload 選択を減らす。

## 対象

入力元として扱う型は以下です。

| 入力元 | `mpfrxx::mpfr_class` への変換 | `mpfrxx::mpc_class` への変換 |
|--------|-------------------------------|------------------------------|
| `dd_real` | 対象 | 実部として明示的に入れる |
| `dd_complex` | 対象外 | 対象 |
| `qd_real` | 対象 | 実部として明示的に入れる |
| `qd_complex` | 対象外 | 対象 |
| `binary80_real` | 対象 | 実部として明示的に入れる |
| `binary80_complex` | 対象外 | 対象 |
| `binary128_real` | 対象 | 実部として明示的に入れる |
| `binary128_complex` | 対象外 | 対象 |
| `gmpxx::mpf_class` | 対象 | `mpfr_class` 経由などで明示的に入れる |
| `double` | 対象 | 実部として明示的に入れる |

逆方向の変換は以下のように、明示的な `cast2...` 関数で行う。

| 入力元 | 出力先 |
|--------|--------|
| `mpfrxx::mpfr_class` | `cast2dd_real`, `cast2qd_real`, `cast2binary80_real`, `cast2binary128_real` |
| `mpfrxx::mpc_class` | `cast2dd_complex`, `cast2qd_complex`, `cast2binary80_complex`, `cast2binary128_complex` |

`TD` / `EDD` はこの方針から外す。  
`gmpxx::mpf_class` / `gmpxx::mpfc_class` は adapter の出力先にはしない。

## API の形

### 外部型から MPFR/MPC へ入れる方向

この方向は高精度側へ値を取り込む方向なので、constructor / assignment /
内部 adapter traits で受けられる形にする。

例:

```cpp
#include <gmpfrxx_mkII/mpfrxx_mkII.h>
#include <gmpfrxx_mkII/mpcxx_mkII.h>
#include <gmpfrxx_mkII/adapters/dd_real.hpp>
#include <gmpfrxx_mkII/adapters/dd_complex.hpp>

using mpfrxx::mpfr_class;
using mpfrxx::mpc_class;

mpfr_class r;
dd_real x = ...;
r = x;

mpc_class z;
dd_complex c = ...;
z = c;
```

実数を `mpc_class` に入れる場合は、実部に入れて虚部を 0 にする、という意味を
呼び出し側で明確にする。曖昧になりやすい箇所では、いったん `mpfr_class` に変換してから
`mpc_class` を作る形がよい。

例:

```cpp
gmpxx::mpf_class f = ...;
mpfrxx::mpfr_class real_part = f;
mpfrxx::mpfr_class imag_part = 0.0;
mpfrxx::mpc_class z(real_part, imag_part);
```

### MPFR/MPC から外部型へ戻す方向

この方向は精度が落ちる可能性があるため、暗黙変換や assignment ではなく、
明示的な `cast2...` 関数だけにする。

例:

```cpp
mpfrxx::mpfr_class r = ...;

dd_real dx = mpfrxx::cast2dd_real(r);
qd_real qx = mpfrxx::cast2qd_real(r);
gmpfrxx_mkII::adapters::binary80_real b80 = mpfrxx::cast2binary80_real(r);
gmpfrxx_mkII::adapters::binary128_real b128 = mpfrxx::cast2binary128_real(r);

mpfrxx::mpc_class z = ...;

dd_complex dz = mpfrxx::cast2dd_complex(z);
qd_complex qz = mpfrxx::cast2qd_complex(z);
gmpfrxx_mkII::adapters::binary80_complex z80 = mpfrxx::cast2binary80_complex(z);
gmpfrxx_mkII::adapters::binary128_complex z128 = mpfrxx::cast2binary128_complex(z);
```

`mpfr_class` / `mpc_class` から外部型へ戻す constructor や assignment は想定しない。

## DD/QD の変換

`dd_real` は 2 成分、`qd_real` は 4 成分を持つので、MPFR へ入れるときは
destination precision の `mpfr_class` 上で成分を足し込む。

概念的には以下の形です。

```cpp
mpfrxx::mpfr_class dd_conved;
dd_conved = x[0];
dd_conved += x[1];

mpfrxx::mpfr_class qd_conved;
qd_conved = y[0];
qd_conved += y[1];
qd_conved += y[2];
qd_conved += y[3];
```

`dd_complex` / `qd_complex` は、実部と虚部をそれぞれ同じ方針で `mpfr_class` に変換し、
その 2 つから `mpc_class` を構成する。

逆方向は明示関数のみ。

```cpp
dd_real dx = mpfrxx::cast2dd_real(r);
qd_real qx = mpfrxx::cast2qd_real(r);

dd_complex dz = mpfrxx::cast2dd_complex(z);
qd_complex qz = mpfrxx::cast2qd_complex(z);
```

## binary80/binary128 の変換

`binary80` / `binary128` は、core の expression scalar leaf として広く受けるのではなく、
adapter 型として扱う。これにより、`long double` や compiler-specific な floating 型を
不用意に式テンプレートへ入れない。

`binary80` は platform の `long double` が 80-bit extended precision の場合に使う
adapter とする。

`binary128` は、可能なら MPFR の float128 API を使う。

```text
mpfr_set_float128
mpfr_get_float128
```

例:

```cpp
using gmpfrxx_mkII::adapters::binary80_real;
using gmpfrxx_mkII::adapters::binary128_real;

binary80_real x80 = ...;
binary128_real x128 = ...;

mpfrxx::mpfr_class r80 = x80;
mpfrxx::mpfr_class r128 = x128;

binary80_real y80 = mpfrxx::cast2binary80_real(r80);
binary128_real y128 = mpfrxx::cast2binary128_real(r128);
```

complex 版も同じ方針で、`binary80_complex` / `binary128_complex` から
`mpc_class` へ入れ、戻すときは `cast2binary80_complex()` /
`cast2binary128_complex()` を使う。

## mpf_class と double

`gmpxx::mpf_class` と `double` は、`mpfr_class` / `mpc_class` への入力元として扱う。
ただし adapter の出力先にはしない。

```cpp
gmpxx::mpf_class f = ...;
mpfrxx::mpfr_class r = f;

double d = 1.25;
mpfrxx::mpfr_class rd = d;
```

`mpc_class` へ入れる場合は、実部として入れることを明示する。

```cpp
mpfrxx::mpfr_class real_part = f;
mpfrxx::mpfr_class imag_part = 0.0;
mpfrxx::mpc_class z(real_part, imag_part);
```

`mpfr_class` / `mpc_class` から `double` へ戻す場合も、呼び出し側で明示的な抽出 API
または明示 cast を使う。これは DD/QD/binary adapter の `cast2...` 系とは別扱いにする。

## mpfrc++ 風 mixed overload との違い

この interop は、mpfrc++ 風に外部型との mixed operator を広く提供するものではない。

提供しないものの例:

```cpp
mpfr_class - dd_real
qd_complex - mpc_class
mpc_class + binary128_complex
```

推奨する書き方は、境界で明示的に MPFR/MPC 側へ寄せてから計算し、最後に必要なら
`cast2...` で外部型へ戻す形です。

```cpp
dd_complex a = ...;
dd_complex b = ...;

mpfrxx::mpc_class za = a;
mpfrxx::mpc_class zb = b;
mpfrxx::mpc_class zr = za + zb;

dd_complex out = mpfrxx::cast2dd_complex(zr);
```

## 対象外

この方針では以下を対象外にする。

- `TD` / `EDD` adapter;
- `mpfr_class` / `mpc_class` から `gmpxx::mpf_class` / `gmpxx::mpfc_class` への変換;
- `gmpxx::mpf_class` / `gmpxx::mpfc_class` から DD/QD/binary adapter 型への変換;
- MPFR/MPC と DD/QD/binary adapter 型の mixed arithmetic operator;
- `mpfr_class` / `mpc_class` から外部型への暗黙的・assignment 的な lossy conversion。
