let
  pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation {
	pname = "mydraw";
	version = "1.0";
	src = /home/asrcpq/xdg/misc/test.tar.gz;

	buildInputs = [
		pkgs.qt5.qtbase
		pkgs.cmake
	];
	nativeBuildInputs = [ pkgs.qt5.wrapQtAppsHook ];

	configurePhase = ''
		cmake .
	'';

	buildPhase = ''
		make
	'';
}
