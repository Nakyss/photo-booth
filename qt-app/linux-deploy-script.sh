#!/usr/bin/env bash
set -euo pipefail

LOGFILE="$(pwd)/make-appimage.log"
# Supprimer ancien log si présent
: > "$LOGFILE"

# Fonction d'aide pour écrire en stdout+log
log() { echo "$*"; echo "$*" >>"$LOGFILE"; }

# Active debug (les commandes exécutées s'afficheront)
exec 3>&1
log "=== Lancement du script $(date) ==="
set -x

APPNAME="Photo-booth"
BUNDLE_DIR="$(pwd)/${APPNAME}-x86_64.AppDir"
EXE_SRC="$(pwd)/build/Desktop_Qt_6_10_0-Release/photo-booth"
ICON_SRC="$(pwd)/data/icons/Photo-booth.png"
LINUXDEPLOYQT="$(pwd)/linuxdeployqt.AppImage"

# Vérifications rapides
if [ ! -f "$EXE_SRC" ]; then
  log "ERREUR: exécutable introuvable : $EXE_SRC"
  echo
  echo "Voir $LOGFILE pour détails. Appuie sur Entrée pour quitter..."
  read -r
  exit 2
fi

log "Préparation AppDir : $BUNDLE_DIR"
rm -rf "$BUNDLE_DIR"
mkdir -p "$BUNDLE_DIR/usr/bin" "$BUNDLE_DIR/usr/share/icons/hicolor/256x256/apps" "$BUNDLE_DIR/usr/share/applications"

log "Copie de l'exécutable"
cp "$EXE_SRC" "$BUNDLE_DIR/usr/bin/$APPNAME"
chmod +x "$BUNDLE_DIR/usr/bin/$APPNAME"
stat -c '%A %s %n' "$BUNDLE_DIR/usr/bin/$APPNAME" >>"$LOGFILE"

log "Copie icône (si existe)"
if [ -f "$ICON_SRC" ]; then
  cp "$ICON_SRC" "$BUNDLE_DIR/${APPNAME}.png" 2>>"$LOGFILE"
  log "Icône copiée"
else
  log "Aucune icône trouvée à $ICON_SRC — pas bloquant"
fi

DESKTOP_CONTENT="[Desktop Entry]
Name=${APPNAME}
Exec=${APPNAME}
Icon=${APPNAME}
Type=Application
Categories=Utility;Graphics;
Terminal=false
"
echo "$DESKTOP_CONTENT" > "$BUNDLE_DIR/${APPNAME}.desktop"
echo "$DESKTOP_CONTENT" > "$BUNDLE_DIR/usr/share/applications/${APPNAME}.desktop"

# Télécharger linuxdeployqt si absent
if [ ! -f "$LINUXDEPLOYQT" ]; then
  log "Téléchargement de linuxdeployqt.AppImage..."
  if command -v wget >/dev/null 2>&1; then
    wget -q -O linuxdeployqt.AppImage "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" >>"$LOGFILE" 2>&1 || true
  elif command -v curl >/dev/null 2>&1; then
    curl -sL -o linuxdeployqt.AppImage "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" >>"$LOGFILE" 2>&1 || true
  else
    log "ERREUR: ni wget ni curl disponibles pour télécharger linuxdeployqt."
    echo "Voir $LOGFILE. Appuie sur Entrée pour quitter..."
    read -r
    exit 3
  fi
fi

# permissions
if [ -f linuxdeployqt.AppImage ]; then
  chmod +x linuxdeployqt.AppImage
else
  log "ERREUR: linuxdeployqt.AppImage introuvable même après tentative de téléchargement."
  echo "Voir $LOGFILE. Appuie sur Entrée pour quitter..."
  read -r
  exit 4
fi

# Lancer linuxdeployqt et logger la sortie
log "Exécution : ./linuxdeployqt.AppImage ${BUNDLE_DIR}/usr/bin/${APPNAME} -appimage -verbose=2"
./linuxdeployqt.AppImage "${BUNDLE_DIR}/usr/bin/${APPNAME}" -appimage -verbose=2 >>"$LOGFILE" 2>&1 || {
  log "linuxdeployqt a retourné une erreur. Voir $LOGFILE pour la sortie complète."
  echo
  echo "=== dernières 80 lignes du log ==="
  tail -n 80 "$LOGFILE" | sed -n '1,80p'
  echo
  echo "Appuie sur Entrée pour quitter..."
  read -r
  exit 5
}

# recherche AppImage générée
FOUND=$(find . -maxdepth 2 -type f -name "*.AppImage" -print | sort)
if [ -n "$FOUND" ]; then
  log "AppImage(s) trouvée(s) :"
  echo "$FOUND" | tee -a "$LOGFILE"
  ls -lh $FOUND | tee -a "$LOGFILE"
else
  log "Aucune AppImage trouvée (vérifier $LOGFILE)"
  echo
  echo "Voir $LOGFILE pour détails. Appuie sur Entrée pour quitter..."
  read -r
  exit 6
fi

log "Terminé avec succès. Appuie sur Entrée pour quitter..."
read -r
