class Moose < Formula
  include Language::Python::Virtualenv

  desc "Multiscale Object Oriented Simulation Environment"
  homepage "http://moose.ncbs.res.in"
  url "https://github.com/BhallaLab/moose-core/archive/3.1.2.tar.gz"
  sha256 "a542983e4903b2b51c79f25f4acc372555cbe31bc0b9827302a6430fad466ef7"
  head "https://github.com/BhallaLab/moose-core.git"

  bottle do
    cellar :any
    sha256 "6077f886560480c956270f855cf9576a3e8261c5f2ea064117c3483f74a84462" => :sierra
    sha256 "a637de34ce0b92f16afc120ecb2e0e4aff8f8a2e6a2ada5521ee01cf7ccdca9e" => :el_capitan
    sha256 "1bb0712ef178577a3c44190be8f21f894cddc66ce03f742d768e44371425dce7" => :yosemite
    sha256 "a62366e1e1de37c13dec6d2b7f91dc63f8b40ab460e35b31a4d94507a0df6219" => :x86_64_linux
  end

  depends_on "cmake" => :build
  depends_on "gsl"
  depends_on "hdf5"
  depends_on :python if MacOS.version <= :snow_leopard
  depends_on "numpy"

  def install
    (buildpath/"VERSION").write("#{version}\n")
    # FindHDF5.cmake needs a little help
    ENV.prepend "LDFLAGS", "-lhdf5 -lhdf5_hl"

    args = std_cmake_args
    args << "-DCMAKE_SKIP_RPATH=ON"
    mkdir "_build" do
      system "cmake", "..", "-DPYTHON_EXECUTABLE:FILEPATH=#{which("python")}", *args
      system "make"
      system "ctest", "--output-on-failure"
    end

    Dir.chdir("_build/python") do
      system "python", *Language::Python.setup_install_args(prefix)
    end
  end

  def caveats; <<-EOS.undent
    You need to install `networkx` and `python-libsbml` using python-pip. Open terminal
    and execute following command:
      $ pip install python-libsbml networkx
    EOS
  end

  test do
    system "python", "-c", "import moose"
  end
end
