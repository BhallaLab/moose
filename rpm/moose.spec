# spec file for package moose
#
# Copyright (c) 2009-2017 Upinder S. Bhalla <bhalla@ncbs.res.in> and NCBS Bangalore
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

%global version 3.1.3
#%define _unpackaged_files_terminate_build 0 

# When ON, build MOOGLI extention as well.
%define with_moogli ON

Name: moose

# fixme: build debuginfo if possible with given complicated cmake calls.
%define debug_package %{nil}

Group: Applications/Biology
Summary: Neuronal modeling software spanning molecules, electrophysiology and networks

Version: %{version}
Release: 1%{?dist}
Url: http://github.com/BhallaLab/moose
Source: moose-%{version}.tar.gz
License: GPL-3.0

BuildRequires: gcc-c++
BuildRequires: cmake
BuildRequires: python-devel
BuildRequires: libxml2-devel


%if 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires: pkg-config
BuildRequires: boost-devel
%else
BuildRequires: pkgconfig
BuildRequires: fdupes
BuildRequires: gsl-devel
%endif

%if 0%{?suse_version} 
BuildRequires: python-sip
%else
BuildRequires: sip-devel
%endif

%if 0%{?suse_version} 
BuildRequires: python-qt4-devel
BuildRequires: libqt4-devel
%else
BuildRequires: PyQt4-devel
BuildRequires: qt4-devel  libjpeg-devel
%endif

%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires: numpy atlas
%else
BuildRequires: python-numpy-devel
%endif

%if 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
Requires: PyQt4
%else
Requires: python-qt4
%endif

Requires: python-networkx
Requires: python-matplotlib
Requires: python-lxml
Requires: libxml2
Requires: gsl >= 1.16

%if 0%{?fedora_version} || 0%{?suse_version} || 0%{?centos_version} || 0%{?scientificlinux_version}
Requires: numpy 
%else
Requires: python-numpy
%endif

%if 0%{?suse_version} || 0%{?rhel_version} 
Requires: python-qt4
%else
Requires: PyQt4
%endif


%description
MOOSE is the Multiscale Object-Oriented Simulation Environment. It is designed
to simulate neural systems ranging from biochemical signaling to complex models
of single neurons, circuits, and large networks. A typical use case is to model
neural activity-driven synaptic plasticity, in which network activity to
synapses triggers biochemical reaction-diffusion events, which in turn modulate
ion channels to give rise to synaptic plasticity.

%prep
%setup -q

%build
# Explicitly adding flags.
mkdir -p _build
cd _build 
cmake \
          -DWITH_DOC=OFF \
          -DWITH_GUI=ON \
          -DCMAKE_INSTALL_PREFIX=%{buildroot}/usr \
          -DCMAKE_INSALL_LIBEXEC=%_libexecdir \
          -DCMAKE_C_FLAGS=%optflags \
          -DCMAKE_CXX_FLAGS=%optflags \
          -DMOOSE_VERSION=%version \
          -DCMAKE_BUILD_TYPE="Release|RelWithDebugInfo" \
          -DCMAKE_INSTALL_DO_STRIP=0 \
          ..

make VERBOSE=1 %{?_smp_mflags}

%install
cd _build
make install
# Note: direct installation causes following
# http://lists.opensuse.org/opensuse-factory/2012-01/msg00235.html
find "%{buildroot}%{python_sitelib}" -type f -name "*.pyc" -exec %__rm {} \;
%__python -c 'import compileall; 
compileall.compile_dir("%{buildroot}/%{python_sitelib}/"
    , ddir="%{python_sitelib}/", force=1)'

# Create a launcher.
GUIBIN=%{buildroot}/%{_prefix}/bin/moosegui
cat > $GUIBIN <<EOF
#!/bin/sh
cd %{_prefix}/lib/moose/gui && python mgui.py & 
EOF
chmod a+x $GUIBIN

%files 
%defattr(-,root,root)
/usr/bin/moosegui
%defattr(-,root,root)
%{python_sitelib}/*
%dir /usr/lib/moose
/usr/lib/moose/gui
/usr/lib/moose/moose-examples
