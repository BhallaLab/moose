class Moose < Formula
  desc "Multiscale Object Oriented Simulation Environment"
  homepage "http://moose.ncbs.res.in"
  url "https://github.com/BhallaLab/moose/archive/master.tar.gz"
  version "3.1.0"

  option "with-gui", "Enable gui support"

  depends_on "cmake" => :build
  depends_on "gsl"
  depends_on "hdf5"

  depends_on "matplotlib" => :python
  depends_on "python" if MacOS.version <= :snow_leopard

  depends_on "pyqt" => :recommended

  def install
    args = std_cmake_args
    args << "-DWITH_SBML=ON"

    # Use libsbml-5.9.0 which is shipped with moose. Use static linking
    # mechanism. DO NOT rely on macport version.
    # if build.with?("libsbml")
    #   args << "-DWITH_SBML=ON"
    # end

    mkdir "_build" do
      system "cmake", "..", *args
      system "make"
    end

    Dir.chdir("moose-core/python") do
      system "python", *Language::Python.setup_install_args(prefix)
    end

    if build.with?("gui")
      (lib/"moose").install "moose-gui"
      (lib/"moose").install "moose-examples"

      # A wrapper script to launch moose gui. Do we need to set PYTHONPATH here,
      # when prefix is not /usr/local?
      (bin/"moosegui").write <<-EOS.undent
        #!/bin/bash
        GUIDIR="#{lib}/moose/moose-gui"
        (cd ${GUIDIR} && python mgui.py)
      EOS
      chmod 0755, bin/"moosegui"
    end
  end

  def caveats
    <<-EOS.undent
    Please install python-networkx and python-suds-jurko using pip to complete
    the dependencies.

    $ pip install networkx suds-jurko

    EOS
  end

  test do
    ## This will not work on Travis
    # system "#{HOMEBREW_PREFIX}/bin/python", "-c", "import moose"
    system "python", "-c", "import moose"
  end
end
