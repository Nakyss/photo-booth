#!/usr/bin/env bash
set -euo pipefail

### Install script pour Photo-booth (Basé sur AppDir)
APPNAME="Photo-booth"
INSTALL_DIR="/opt/${APPNAME}"
BIN_LINK="/usr/local/bin/photo-booth"
DESKTOP_FILE="/usr/share/applications/photo-booth.desktop"
ICON_DEST="/usr/share/icons/hicolor/256x256/apps/${APPNAME}.png"

# Vérification root
if [ "$(id -u)" -ne 0 ]; then
  echo "Ce script doit être lancé avec sudo."
  exit 1
fi

echo "=== Installation de ${APPNAME} depuis le bundle ==="

# 1. Nettoyer l'ancienne installation
if [ -d "${INSTALL_DIR}" ]; then
  echo "Suppression de l'ancienne version..."
  rm -rf "${INSTALL_DIR}"
fi

# 2. Copier tout le contenu du dossier courant vers /opt/Photo-booth
echo "Copie des fichiers vers ${INSTALL_DIR}..."
mkdir -p "${INSTALL_DIR}"
# On copie tout sauf le script d'install lui-même
cp -a . "${INSTALL_DIR}/"
# On supprime le script install.sh qui a été copié dans /opt par mégarde
rm -f "${INSTALL_DIR}/install.sh"

# 3. Gestion des droits
# AppRun est le point d'entrée clé créé par linuxdeployqt
chmod +x "${INSTALL_DIR}/AppRun"
# On s'assure que le binaire interne est exécutable aussi
if [ -f "${INSTALL_DIR}/usr/bin/${APPNAME}" ]; then
    chmod +x "${INSTALL_DIR}/usr/bin/${APPNAME}"
fi

# 4. Créer le lien symbolique dans /usr/local/bin
# IMPORTANT : On pointe vers 'AppRun', car c'est lui qui charge les libs Qt incluses !
echo "Création du lien symbolique..."
rm -f "${BIN_LINK}"
ln -s "${INSTALL_DIR}/AppRun" "${BIN_LINK}"

# 5. Installer l'icône système
# linuxdeployqt met souvent l'icône à la racine sous le nom .DirIcon ou appname.png
ICON_SRC=""
if [ -f "${INSTALL_DIR}/${APPNAME}.png" ]; then
    ICON_SRC="${INSTALL_DIR}/${APPNAME}.png"
elif [ -f "${INSTALL_DIR}/.DirIcon" ]; then
    ICON_SRC="${INSTALL_DIR}/.DirIcon"
fi

if [ -n "$ICON_SRC" ]; then
    echo "Installation de l'icône..."
    mkdir -p "$(dirname "${ICON_DEST}")"
    cp -f "$ICON_SRC" "${ICON_DEST}"
    gtk-update-icon-cache -q --force /usr/share/icons/hicolor || true
else
    echo "Aucune icône trouvée dans le bundle (non bloquant)."
fi

# 6. Créer/Copier le fichier .desktop système
# linuxdeployqt crée déjà un .desktop à la racine, on peut le réutiliser ou le recréer
if [ -f "${INSTALL_DIR}/${APPNAME}.desktop" ]; then
    echo "Installation du fichier desktop..."
    cp "${INSTALL_DIR}/${APPNAME}.desktop" "${DESKTOP_FILE}"
    
    # On doit souvent corriger la ligne Exec et Icon dans le fichier généré par linuxdeployqt
    # pour qu'elle pointe vers le lien système global
    sed -i "s|^Exec=.*|Exec=${BIN_LINK}|" "${DESKTOP_FILE}"
    sed -i "s|^Icon=.*|Icon=${APPNAME}|" "${DESKTOP_FILE}"
else
    # Fallback si pas de .desktop
    echo "Création manuelle du fichier desktop..."
    cat > "${DESKTOP_FILE}" <<EOF
[Desktop Entry]
Name=${APPNAME}
Exec=${BIN_LINK}
Icon=${APPNAME}
Type=Application
Categories=Graphics;Utility;
Terminal=false
EOF
fi
chmod 644 "${DESKTOP_FILE}"

echo "Installation terminée avec succès !"
echo "Lancez l'application avec : photo-booth"