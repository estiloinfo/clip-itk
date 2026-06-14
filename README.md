# clip-itk

> A compiler and runtime for the **Clipper/xBase** programming language.
> Translates `.prg` source files into native shared libraries and executables,
> with a complete VM and standard library compatible with the Clipper ecosystem.

**Tested on:** Debian 12 · Debian 13 · Gentoo  
**Upstream:** https://sourceforge.net/projects/clip-itk/files/clip-itk/1.2.0/

---

## Table of contents

- [Build dependencies](#build-dependencies)
- [Building and installing](#building-and-installing)
- [Usage](#usage)
- [Documentation](#documentation)
- [Locale utilities](#locale-utilities)


---

## Build dependencies

### Core

| Component | Purpose |
|-----------|---------|
| `gcc`, `make` | Compiler toolchain |
| `flex`, `bison` | Lexer and parser generators |
| `ncurses` | Terminal I/O |
| `zlib` | Compression support |
| `gettext` | `clip_msgfmt` / `clip_msgmerge` locale utilities |
| `readline` | `clip_gdb` debugger |

<details>
<summary>Debian 12 / Debian 13</summary>

```sh
apt-get install git make flex bison libncurses-dev zlib1g-dev gettext libreadline-dev
```

</details>

<details>
<summary>Gentoo</summary>

```sh
emerge -av git flex bison ncurses zlib gettext readline
```

</details>

### Optional libraries

Install only the libraries needed for your target packages.

| Library | Purpose | Debian 12/13 | Gentoo |
|---------|---------|--------------|--------|
| task support | Cooperative multitasking | `libpth-dev` (Debian 12) / `libnpth0-dev` (Debian 13) | `npth` |
| clip-bzip2 | bzip2 compression | `libbz2-dev` | `app-arch/bzip2` |
| clip-com | Serial port I/O (RS-232) | *(no extra dep)* | *(no extra dep)* |
| clip-crypto | TLS / crypto | `libssl-dev` | `openssl` |
| clip-fcgi | FastCGI | `libfcgi-dev` | `fcgi` |
| clip-gzip | gzip compression | *(no extra dep)* | *(no extra dep)* |
| clip-mysql | MySQL client | `default-libmysqlclient-dev` | `mysql-connector-c` |
| clip-oasis | Nanfor + Netto utility collections | *(no extra dep)* | *(no extra dep)* |
| clip-odbc | ODBC | `unixodbc-dev` | `unixODBC` |
| clip-postgres | PostgreSQL client | `postgresql-server-dev-all` | `postgresql` |
| clip-postscript | PostScript output | *(no extra dep)* | *(no extra dep)* |
| clip-r2d2 | Accounting / reporting framework | *(no extra dep)* | *(no extra dep)* |
| clip-rtf | RTF document generation | *(no extra dep)* | *(no extra dep)* |
| clip-xml | XML / Expat | `libexpat1-dev` | `expat` |

### Legacy / omitted libraries

Source code is present but these libraries are excluded from the build.

<details>
<summary>Excluded from build (click to expand)</summary>

| Library | Reason |
|---------|--------|
| `clip-gtk2` | GTK2 removed from Debian 13+ |
| `clip-glade2` | GTK2 removed from Debian 13+ |
| `clip-ui` | Requires `clip-xml` installed as a shared library |
| `clip-gd` | No `libgd-dev` available in current distros |
| `clip-oracle` | Proprietary Oracle client headers required |
| `clip-interbase` | InterBase/Firebird dev headers required |
| `clip-ctcom` | Earlier serial port attempt — incomplete CLIP bindings; superseded by `clip-com` |
| `clip-dbtcp` | Database-over-TCP layer — not wired into build |

</details>

---

## Building and installing

### Local install (single user)

Installs to `$HOME/bin`. `CLIPROOT` defaults to `../cliproot`.

```sh
./make local
```

### System-wide install

Installs to `/usr/local/bin`. `CLIPROOT` defaults to `/usr/local/clip`.

```sh
./make system
```

Custom `CLIPROOT`:

```sh
./make system CLIPROOT=/opt/clip
```

---

## Usage

Create a source file `hello.prg`:

```clipper
? "Hello, world!"
```

Compile and run:

```sh
clip -e -ohello hello.prg
./hello
```

> **Note:** `-o` and the output name must be written without a space (`-ohello`, not `-o hello`).

| Flag | Effect |
|------|--------|
| `-e` | Produce a standalone executable |
| `-e` omitted | Compile to a shared library (loadable module) |
| `-ohello` | Set output filename |
| `-g` | Include debug information |

---

## Documentation

HTML documentation is generated in **English** and **Russian** from DocBook XML sources in `doc/sgml/{en,ru}/`, using `xsltproc` with the DocBook XSL NS stylesheet.

### Dependencies

<details>
<summary>Debian 12 / Debian 13</summary>

```sh
apt-get install xsltproc docbook-xsl-ns docbook-xml
```

</details>

<details>
<summary>Gentoo</summary>

```sh
emerge -av libxslt docbook-xsl-ns-stylesheets
```

</details>

### Building

```sh
cd doc
make CLIPROOT=/usr/local/clip html
```

> For a local install, replace `/usr/local/clip` with the path used during `make local` (defaults to `../cliproot`).

Output is written to `doc/html/{en,ru}/`. Alternatively, use the top-level script:

```sh
./mkdoc
```

---

## Locale utilities

| Tool | Purpose |
|------|---------|
| `clip_msgmerge` | Merges `.pot` template files with `.po` translation files per locale |
| `clip_msgfmt` | Compiles `.po` files into binary `.mo` catalogs loaded at runtime via `loadModuleMsg()` |

These follow the standard GNU gettext workflow within the CLIP ecosystem.
