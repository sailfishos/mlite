Name:       mlite-qt5
Summary:    Useful classes originating from MeeGo Touch
Version:    0.4.4
Release:    1
License:    LGPLv2
URL:        https://github.com/sailfishos/mlite/
Source0:    %{name}-%{version}.tar.bz2
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(dconf)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  qt5-qttools-linguist

%description
Select set of useful classes from meegotouch that do not require
taking in all of meegotouch.


%package notificationtool
Summary:    mlite notification tool package
Requires:   %{name} = %{version}-%{release}

%description notificationtool
%{summary}.


%package devel
Summary:    mlite development package
Requires:   %{name} = %{version}-%{release}

%description devel
Development files needed for using the mlite library


%package tests
Summary:    Test suite for mlite-qt5
Requires:   %{name} = %{version}-%{release}

%description tests
Test suite for mlite-qt5.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 VERSION=%{version}
%make_build

%install
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%license LICENSE.LGPL
%{_libdir}/libmlite5.so.*
%{_libexecdir}/mliteremoteaction

%files notificationtool
%{_bindir}/mlitenotificationtool

%files devel
%{_includedir}/mlite5
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libmlite5.so

%files tests
/opt/tests/mlite-qt5/*
