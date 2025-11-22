#!/usr/bin/env bash
set -e
APPNAME="Photo-booth"
BUNDLE_DIR="$(pwd)/${APPNAME}.AppDir"
EXE_SRC="$(pwd)/build/Desktop_Qt_6_10_0-Release/photo-booth"   # <-- remplace

rm -rf "$BUNDLE_DIR"
mkdir -p "$BUNDLE_DIR/usr/bin"
cp "$EXE_SRC" "$BUNDLE_DIR/usr/bin/$APPNAME"
# facultatif : icône et desktop
mkdir -p "$BUNDLE_DIR/usr/share/icons/hicolor/128x128/apps"
cp "$(pwd)/data/icons/logo.png" "$BUNDLE_DIR/usr/share/icons/hicolor/128x128/apps/${APPNAME}.png"
cat > "$BUNDLE_DIR/${APPNAME}.desktop" <<EOF
[Desktop Entry]
Name=$APPNAME
Exec=$APPNAME
Icon=$APPNAME
Type=Application
Categories=Utility;
EOF

# Télécharger linuxdeployqt si pas présent
if [ ! -f linuxdeployqt.AppImage ]; then
  wget -q -O linuxdeployqt.AppImage "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
  chmod +x linuxdeployqt.AppImage
fi


./linuxdeployqt.AppImage "$BUNDLE_DIR/usr/bin/$APPNAME" -appimage

echo "AppImage générée. Teste-la sur une autre distribution."
