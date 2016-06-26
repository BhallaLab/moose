class Moogli < Formula
  desc "3d visualizer of neuronal simulation"
  homepage "http://moose.ncbs.res.in/moogli"
  url "https://github.com/BhallaLab/moogli/archive/macosx.tar.gz"
  version "0.5.0"

  depends_on "open-scene-graph"
  depends_on "python" if MacOS.version <= :snow_leopard
  depends_on "sip"
  depends_on "pyqt"

  def install
    ENV['CFLAGS'] = ""
    ENV['CXXFLAGS'] = ""
    # Copy QtCore.so etc to lib of our app.
    system "./build_on_macosx.sh"
    system "python", "setup.py", "install", "--prefix", prefix
    (prefix/"lib/python2.7/site-packages/moogli/core").install "moogli/core/_moogli.so"
  end

  test do
    system "#{HOMEBREW_PREFIX}/bin/python", "-c", "import moogli"
    system "python", "-c", "import moogli"
  end
end
