workflow "build pymoose" {
  resolves = "build"
  on = "push"
}

action "build" {
  uses = "./"
  args = "install"
  env = {
    CMAKE_GIT_REPO = "https://github.com/dilawar/moose-core",
    CMAKE_INSTALL_DEPS_SCRIPT = ".travis/travis_prepare_linux.sh",
    CMAKE_FLAGS = "-DCMAKE_INSTALL_PREFIX:PATH=/tmp/foo",
    CMAKE_BUILD_THREADS = "4"
  }
}
