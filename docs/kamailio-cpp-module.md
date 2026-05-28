# Creating a C++ Based Module in Kamailio

> **Source:** [https://blog.zaleos.net/kamailio-cpp-module/](https://blog.zaleos.net/kamailio-cpp-module/)
> **Author:** Luis Martin Gil — Zaleos
> **Published:** 27 Mar 2019
>
> This document is a faithful summary of the original article. All credit for the methodology, code structure, and examples goes to the original author. Please refer to the source link for the full text and the companion Git repository.

---

## Introduction

Zaleos works extensively with real-time communication protocols such as SIP and WebRTC, leveraging [Kamailio](https://www.kamailio.org/), an open-source SIP server capable of handling thousands of VoIP calls per second.

This article presents a development approach for writing **C++ modules** within Kamailio, allowing developers to implement business logic in modern, object-oriented C++ while preserving Kamailio's efficient C-based core.

---

## Goal

Create a self-contained C++ project that:

- Builds as a **static or shared library** (`.a` / `.so`) suitable for linking.
- Ships **unit tests** linked against the produced library.
- Ships **example binaries** that demonstrate library usage and can be inspected with tools such as **Valgrind** for memory-leak detection.
- Is bridged to Kamailio via a thin **C interface** consumed by a Kamailio C module.

The project uses **CMake** instead of plain Make because it supports multiple builds from a single source tree and is more portable across platforms.

---

## Project Structure

The example C++ library is named `libzzz`. Its layout:

| Directory      | Purpose                                                |
|----------------|--------------------------------------------------------|
| `src/zzz`      | Core C++ library implementation                        |
| `src/test`     | Unit-test sources (Catch framework)                    |
| `src/examples` | Standalone example programs using the library         |
| `lib`          | Compiled library output (`.a` / `.so`)                 |
| `bin`          | Compiled example/test binaries                         |
| `build`        | Out-of-tree directory where CMake is invoked           |
| `cmake`        | CMake helper modules / build configuration             |

---

## The Core C++ Class

A demonstration class `Core` accepts an `int` and a `std::string`, and exposes:

- A getter for the integer.
- A getter for the string.
- A method returning the integer **incremented by one**.

A small **C interface** (`core-c.hpp` / `core-c.cpp`) wraps the C++ object so that C code (i.e. the Kamailio module) can construct, destroy and call methods on it without seeing any C++ symbols.

---

## Testing

The library uses the [**Catch**](https://github.com/catchorg/Catch2) unit-testing framework, which supports both TDD and BDD styles. Basic tests verify that the `Core` class returns the right values and correctly increments the number.

---

## Build & Run

Typical out-of-tree CMake build:

```bash
mkdir build && cd build
cmake ..
make
```

This produces both a shared and a static variant of `libzzz`, plus the test and example binaries in `bin/`. The binaries can be executed standalone and inspected with Valgrind, e.g.:

```bash
valgrind --leak-check=full ./bin/example
```

---

## The Kamailio Module (`zzz`)

The Kamailio module bridges the compiled `libzzz` to the SIP server.

Responsibilities of the module:

1. **At startup** — instantiate a `Core` object via the C interface.
2. **At shutdown** — destroy the `Core` object.
3. **Expose two parameters** to the Kamailio configuration script: `number` and `text`.
4. **Export the function** `get_incremented_number_mod` for use in routing logic.

The module's `Makefile` invokes the CMake build of `libzzz` so the C++ library is compiled as part of building the Kamailio module.

---

## Kamailio Configuration

Loading the module and setting parameters:

```
loadmodule "zzz.so"
modparam("zzz", "number", 555)
modparam("zzz", "text", "This is a not-used text from cfg")
```

Using the exported function inside a request route:

```
request_route {
    ...
    $var(incremented_number) = "";
    get_incremented_number_mod("$var(incremented_number)");
    sl_send_reply("$var(incremented_number)", "Zaleos Tutorial");
    exit;
}
```

An incoming `INVITE` therefore triggers a SIP **`556`** reply — `555` from `modparam`, incremented to `556` by `libzzz`.

You can capture and inspect the resulting SIP traffic with [**sngrep**](https://github.com/irontec/sngrep).

---

## Summary of the Workflow

1. Develop C++ classes independently, with unit tests, in an isolated CMake project.
2. Expose a **C interface** wrapper over the C++ API.
3. Build the project into a reusable library (`libzzz.a` / `libzzz.so`).
4. Wrap the library inside a **Kamailio C module** that loads the library, manages object lifetime, and exports module functions / parameters.
5. Load the module from `kamailio.cfg` and call its functions from SIP routing logic.

This pattern keeps the business logic in clean, testable C++ code while staying fully compatible with Kamailio's C-based module ABI.

---

## Referenced Tools

- **CMake** — build system.
- **Catch** — C++ unit-testing framework.
- **Valgrind** — memory analysis for the example/test binaries.
- **sngrep** — terminal-based SIP traffic viewer.

---

## Original Resources

- **Article:** <https://blog.zaleos.net/kamailio-cpp-module/>
- **Reference implementation:** <https://github.com/zaleos/post-kamailio-module-cpp>

All content above is derived from the article authored by **Luis Martin Gil** at Zaleos and is reproduced here for educational reference. Refer to the original post for the authoritative version.
