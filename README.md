---
header-includes:
 - \usepackage{siunitx}
 - \usepackage{fullpage}
---
# C++ Component Template Project

# Description

This is a template project directory for developing C++ components (libraries that will be used by other
applications). It includes much of the boiler plate required for setting up new projects including the directory
layout, `cmake` configuration files, and unit tests. If you are starting a new project, clone this repository
and delete the `.git` directory. You can then add your project files and run `git init` to initialize a repository
for your project.

    > git clone http://fermi.fhsu.edu/CD3/CppProjectTemplate.git MyProject
    > cd MyProject
    > rm .git -rf
    > # setup your project...
    > git init
    > git add .
    > git commit -m "initial import"

This template is based on the practices developed when building C++ libraries. It is based on the following
tools/conventions:

- CMake
  - Used for build configuration. This allows your project to be compiled on Linux and Windows without modification.
- C++-11
  - The CMake configuration files assume that your project needs some features that are new to C++-11 standard and
    will automatically determine the compile flags necessary to enable the standard.
- Git
  - Used for version control. This template includes a CMake module that will automatically extract version numbers from
    git tags and write the information to a `version.h` file.
- Catch
  - A small (single header file) unit test framework for C++ (https://github.com/philsquared/Catch). The Catch
    source code is included directly into your project so that you can build unit tests on a new machine without
    installing anything. The CMake configuration file included in the testing directly will automatically detect
    and build new unit tests in the `CatchTests` directory, so all you have to do is write tests and same them
    in that directory.
- Documentation
  - Doxygen
    - A tool for generating documentation from comments in the source code. When writing comments to document your code,
      you should use the Doxygen format so that Doxygen can extract them and build documentation. You only need to write
      documentation for your code in the comments. Technical write-ups and user guides should be in their own files.
  - Technical Write-ups
    - Technical write-ups (write-ups that describe physics, mathematics, numerical methods, etc.) should be written
      using LaTeX and placed in the `doc` directory (preferably in a sub-directory named `write-ups`.
  - User Guide
    - Your project should contain a short user guide that gives information on obtaining and using your project. The
      user guide should be placed in this file (README.md at the top of your project), so that it will automatically
      be displayed by tools like GitLab and GitHub.
- Externals
  - External dependencies can be included with your project to make setting up and using your project easier. For example,
    if your project uses another component library, you could include the libraries source in your project directory so that
    new users don't have to find and install it as well.
  - External dependencies should be placed in their own directory under the `externals` directory. If you do not plan
    on making changes to the external dependency, then it is sufficient to just copy the external project directory into
    the `externals` directory. However, if you will be making modifications that should be pushed "upstream" (bug
    fixes for example), then you should use a tool like [`git subreo`](https://github.com/ingydotnet/git-subrepo)
    to manage your external dependencies.
- Non-invasive Licenses
  - Your project should not depend on any libraries that have restrictive licenses, especially for dependencies that will
    be included directory in your project. For example, if you own a copy of Numerical Recipes and use one of their functions
    to implement a new method you are working on, your code cannot be distributed to anybody else. You will need to 
    re-implement your method uses a different library.
  - It is OK to depend on libraries that are commonly available without worrying about the license. User's of your library
    will be required to install these dependencies on their own, but as long as you only use the library as is (no
    modifications), the libraries license will not affect your code.

## Benefits

There are several benefits to using this project template. Much of the boiler plate code needed to setup a (nice) project
has already been written and is automated. For example, the `CMakeLists.txt` files included in this template contain code
to automatically detect your project's source code and build unit tests that
will be added to the test target. If your project is fairly simple and you put all of your source code in the `src` directory,
then you probably won't have to modify them at all. To build and test your project, you will just have to run:

    > mkdir build
    > cd build
    > cmake ..
    > make
    > make test

A GitLab continuous integration job that runs these commands has been included, so you can setup GitLab to automatically
run these tests every time you commit to the server.

# Conventions

## CMake

Your CMakeLists.txt file should support being called from a another CMakeLists.txt file with
the `add_subdirectory` command. Basically, you just need to make sure that you don't hard code
any paths in such a way that they will break if called as a sub-project.

### Externals

It is very common for a project to depend on one or more other projects.  You
may decide to include external projects directly in your project (see above) to
make building your project easier.  However, you should allow for a user of
your project the option of providing these externals. For example, consider three
projects: A, B, and C. Project B depends on project A, and project C depends on project
B and C:
```
Projects
+-- A
+-- B
|   +-- A
+-- C
    +-- A
    +-- B
        +-- A
```
If project B includes A in its externals directory and project C includes both A and B in
its externals directory, then project C will have two copies of project A. This will cause
`cmake` to complain about the targets defined in project A being defined multiple
times. We would like to have a way to tell project B that it should not include its copy of project A.

This can be accomplished using `cmake`'s `add_subdirectory` command and cache variables.
Cache variables will be stored in the `cmake`'s cache, and can be accessed from separate
projects. To set a cache variable, you use the `set` command with the `CACHE` option:

```
set( NAME VALUE CACHE INTERNAL "A cached value" FORCE)
```

This will set a variables named `NAME` to the value `VALUE` and store it in the cache.
The string following `CACHE` is a description that will be written to the cache
file in a comment, and the `FORCE` option will make sure the variable is overwritten
if it is already in the cache.

So, we can use the cache to let other projects know that our project has already
been configured somewhere, and to determine if any of the external projects we include
have already been configured. Add this line to the end of your `CMakeLists.txt` file.

```
set( ${PROJECT_NAME}_CONFIGURED 1 CACHE INTERNAL "${PROJECT_NAME} Configured:" FORCE)
```

This will create a cache variable that other projects can look for to determine if some
copy of the project has already been configured. The other thing that you can (and should)
do, is write information that is needed to compile your project to the cache. For example,
create a cache variable named `${PROJECT_NAME}_INCLUDE_DIRS` that contains the list of
directories to search for includes. This way, a project that includes your project with
the `add_subdirectory` command can just use the list generated by your `CMakeLists.txt` file
instead of having to figure what directories should be added.

So, in the example example above, project A's 
`CMakeLists.txt` might look like this (only the important parts are shown).

```
...
project(A)
...
generate a list of include directories in a local (not cache) variable named
_${PROJECT_NAME}_INCLUDE_DIRS
...

set( ${PROJECT_NAME}_INCLUDE_DIRS ${_{PROJECT_NAME}_INCLUDE_DIRS} CACHE INTERNAL "${PROJECT_NAME} Include Directories:" FORCE)
set( ${PROJECT_NAME}_CONFIGURED 1 CACHE INTERNAL "${PROJECT_NAME} Configured:" FORCE)
```
Then project B would include project A as a sub-project
```
...
project(B)
...
# generate a list of include directories in variable named _${PROJECT_NAME}_INCLUDE_DIRS
...
# include project A if it has not been configured yet
if( NOT A_CONFIGURED )
add_subdirectory(A)
endif()
# append project A includes to project B includes
list( APPEND _${PROJECT_NAME}_INCLUDE_DIRS ${A_INCLUDE_DIRS} )
...

set( ${PROJECT_NAME}_INCLUDE_DIRS ${_{PROJECT_NAME}_INCLUDE_DIRS} CACHE INTERNAL "${PROJECT_NAME} Include Directories:" FORCE)
set( ${PROJECT_NAME}_CONFIGURED 1 CACHE INTERNAL "${PROJECT_NAME} Configured:" FORCE)
```

Now project C can include both A and B. As long as it includes A first, the copy of A
in B will be skipped by B.
```
add_subdirectory(A)
add_subdirectory(B)
```
And actually, project C should wrap these in checks to see if A and B have already been
configured, so that it can be included as a sub-project as well.
