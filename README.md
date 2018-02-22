# Project Spark++ - Master thesis of a game engineering student

## What is project Spark?

With project Spark I'm going to compare the creation of a modular game engine in C++ with one in Rust, a new and safe system programming language. I will implement selected sub-modules, that are typical for a performant game engine, both in C++ and Rust and measure the resulting solutions regarding performance and memory access patterns. On one hand this project shall introduce techniques used to implement high-performant engine code in C++ and on the other hand it shall proof whether it is possible to create the same modules using Rust's safety while maintaining a comparable performance.

## What will be implemented?

The sub-modules I choose are:

- Memory Management (MemoryArenas providing different allocators and debug capabilities)
- Containers (commonly used at runtime/for tooling, Vector, HashMap, Fixed-sized array, trees, ...)
- Entity Component System (ECS and systems to update/create components)

## What is contained in this repository?

This repo contains the C++ implementation of project Spark - called Spark++. Spark++ will include the three sub-modules
which are built as static libaries, a benchmarking application to run several measurements and micro benchmarks and a unit-test
project (using google-test) to improve the development process by using TDD and ensure a stable API of the modules.

## What is not included in this repository?

This repository does not include the other part of project Spark - RustySpark. RustySpark is the Rust implementation of the sub-modules which is hosted in a separate repository. RustySpark uses `cargo` as build system and also includes the three module projects as static libraries, a benchmark application as well as unit-tests embedded into the sub-module crates (thanks to `cargo test`).

## Where can I find the results?

As soon as the results are ready a third, general project Spark repository will be created that contains both, Spark++ and RustySpark, as submodules. Beside the projects it will feature the Master's thesis and all measurements and findings of the process.