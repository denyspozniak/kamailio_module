# my_module — A Minimal Kamailio Module in C

[![Build](https://github.com/denyspozniak/kamailio_module/actions/workflows/build.yml/badge.svg)](https://github.com/denyspozniak/kamailio_module/actions/workflows/build.yml)
![Kamailio](https://img.shields.io/badge/Kamailio-master-2C8EBB?logo=asterisk&logoColor=white)
![Language](https://img.shields.io/badge/C-99-A8B9CC?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black)
![Protocol](https://img.shields.io/badge/protocol-SIP-FF6F00)
![Status](https://img.shields.io/badge/status-learning%20project-success)

A tiny, hands-on example of a **Kamailio** module written in C. It exists to make one thing easy:
**get from "I have never written a Kamailio module" to "I can read a SIP header from a route script"** — without any extra ceremony.

If you are exploring Kamailio's module API, looking for a clean skeleton to copy, or just want to see how data flows from a SIP request into your own C code, you are in the right place.

---

## What it does

The module registers two functions that you can call from `kamailio.cfg`:

| Function          | What it reads                          | What it does                                  |
|-------------------|----------------------------------------|-----------------------------------------------|
| `get_sip_callid()`| The `Call-ID` header of the SIP message| Trims it and logs it.                         |
| `get_sip_to()`    | The **Request-URI** of the first line  | Logs it as `%.*s`. (See note below.)          |

> Heads-up: `get_sip_to()` is currently a learning stub — it logs the Request-URI rather than the actual `To:` header. The "real" To-header parsing path (using `get_to()` + `parse_uri()`) is included in the source as a commented block, so you can flip the implementation on and study it.

---

## Project layout

```
.
├── my_module.c     # Module source — exports, init function, route functions
├── my_module.cfg   # Minimal Kamailio config that loads and uses the module
├── Makefile        # Hooks into Kamailio's module build system
├── docs/
│   └── kamailio-cpp-module.md   # Companion tutorial — see "Documentation" below
└── README.md       # You are here
```

---

## Prerequisites

You need the Kamailio source tree on the machine where you build the module — the `Makefile` expects to be invoked from `<kamailio>/src/modules/<your-module>/` and pulls in Kamailio's `Makefile.defs` / `Makefile.modules`.

On RHEL / CentOS / Rocky / Alma:

```bash
yum install -y flex bison gcc gcc-c++
```

On Debian / Ubuntu:

```bash
apt-get install -y flex bison gcc g++ make
```

You also need the Kamailio sources checked out at, for example, `/usr/src/kamailio`.

---

## Build

1. Place this module's directory inside the Kamailio source tree:

   ```
   /usr/src/kamailio/src/modules/my_module/
   ```

2. From the Kamailio source root, build and install just this module to a sandboxed prefix so you don't touch your system Kamailio:

   ```bash
   export KAM_PREFIX=/usr/src/test

   make install_modules="my_module" cfg
   make Q=0 all
   make modules
   make PREFIX=$KAM_PREFIX install
   ```

`Q=0` makes the build chatty, which is handy when you are learning the toolchain.

---

## Run

Launch the sandboxed Kamailio with this module's config, with verbose logging on stderr:

```bash
/usr/src/test/sbin/kamailio \
    -f /usr/src/kamailio/src/modules/my_module/my_module.cfg \
    -L /usr/src/test/lib64/kamailio/modules/ \
    -Y /tmp \
    -E -e -dd -DD
```

Flag cheat sheet:

| Flag | Meaning                                     |
|------|---------------------------------------------|
| `-f` | Path to the config file                     |
| `-L` | Where to look for module `.so` files        |
| `-Y` | Runtime / working directory                 |
| `-E` | Log to stderr instead of syslog             |
| `-e` | Colorize log output                         |
| `-dd`| Increase debug verbosity (twice)            |
| `-DD`| Run in foreground, no fork                  |

---

## Try it

With the server running, send any SIP request to `127.0.0.1:5060` (UDP). For example, with [`sipsak`](https://github.com/nils-ohlmeier/sipsak):

```bash
sipsak -s sip:test@127.0.0.1:5060
```

You should see the module log the Request-URI it pulled out of the message.

---

## The config in plain English

`my_module.cfg` is intentionally tiny:

```cfg
debug=2
log_stderror=yes
listen=udp:127.0.0.1:5060
loadmodule "my_module.so"

request_route{
    // get_sip_callid();
    get_sip_to();
    exit;
}
```

Every incoming request hits `request_route`, which calls into our module and exits. No relaying, no replies — just enough surface area to observe the module being invoked.

---

## Going further — write a C++ module

Once you are comfortable with a plain C module, the natural next step is putting your business logic in C++ and bridging it to Kamailio through a thin C interface.

A companion walkthrough lives in **[`docs/kamailio-cpp-module.md`](docs/kamailio-cpp-module.md)**. It covers:

- Building a standalone C++ library (`libzzz`) with CMake.
- Exposing a clean C interface over your C++ classes.
- Wrapping that library inside a Kamailio module.
- Loading it from `kamailio.cfg` and calling its functions from a route.

That document is based on the excellent post by **Luis Martin Gil (Zaleos)** — full source: <https://blog.zaleos.net/kamailio-cpp-module/>.

---

## Useful links

- [Kamailio — official site](https://www.kamailio.org/)
- [Kamailio — module development docs](https://www.kamailio.org/wiki/devel/modules)
- [Zaleos — Creating a C++ based module in Kamailio](https://blog.zaleos.net/kamailio-cpp-module/)
- [Reference C++ module repo](https://github.com/zaleos/post-kamailio-module-cpp)
