#!/usr/bin/env bash
set -euo pipefail

APPNAME="Photo-booth"
INSTALL_DIR="/opt/${APPNAME}"
BIN_LINK="/usr/local/bin/photo-booth"
DESKTOP_FILE="/usr/share/applications/photo-booth.desktop"
ICON_PATH="/usr/share/icons/hicolor/256x256/apps/${APPNAME}.png"
AUTOSTART_USER="$HOME/.config/autostart/${APPNAME}.desktop"

LOGFILE="$(pwd)/uninstall_${APPNAME}.log"
: > "${LOGFILE}"
log(){ echo "$*"; echo "$*" >> "${LOGFILE}"; }

usage(){
  cat <<EOF
Usage: sudo ./uninstall.sh [--purge]
  --purge   : supprimer aussi les données utilisateur (~/.config/${APPNAME}, ~/.local/share/${APPNAME})
EOF
}

if [ "${1:-}" = "--help" ] || [ "${1:-}" = "-h" ]; then
  usage
  exit 0
fi

PURGE=false
if [ "${1:-}" = "--purge" ]; then
  PURGE=true
fi

if [ "$(id -u)" -ne 0 ]; then
  echo "Ce script doit être lancé avec sudo." >&2
  exit 2
fi

log "=== Désinstallation de ${APPNAME} (purge=${PURGE}) ==="

# 1) Supprimer le lanceur
if [ -L "${BIN_LINK}" ] || [ -f "${BIN_LINK}" ]; then
  log "Suppression du lanceur ${BIN_LINK}"
  rm -f "${BIN_LINK}"
else
  log "Aucun lanceur ${BIN_LINK} trouvé"
fi

# 2) Supprimer .desktop système
if [ -f "${DESKTOP_FILE}" ]; then
  log "Suppression du .desktop ${DESKTOP_FILE}"
  rm -f "${DESKTOP_FILE}"
else
  log "Aucun .desktop système trouvé"
fi

# 3) Supprimer icône système
if [ -f "${ICON_PATH}" ]; then
  log "Suppression de l'icône ${ICON_PATH}"
  rm -f "${ICON_PATH}"
  if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -q --force /usr/share/icons/hicolor || true
  fi
else
  log "Aucune icône système trouvée"
fi

# 4) Supprimer dossier d'installation (réellement)
if [ -d "${INSTALL_DIR}" ]; then
  log "Suppression du dossier d'installation ${INSTALL_DIR}"
  rm -rf "${INSTALL_DIR}"
else
  log "Aucun dossier d'installation ${INSTALL_DIR} trouvé"
fi

# 5) Supprimer autostart utilisateur
if [ -f "${AUTOSTART_USER}" ]; then
  log "Suppression du fichier d'autostart utilisateur ${AUTOSTART_USER}"
  rm -f "${AUTOSTART_USER}"
else
  log "Aucun autostart utilisateur trouvé (${AUTOSTART_USER})"
fi

# 6) Mise à jour des caches (silencieuse)
if command -v update-desktop-database >/dev/null 2>&1; then
  update-desktop-database >/dev/null 2>&1 || true
fi

# 7) Option purge: supprimer les données utilisateur
if [ "${PURGE}" = true ]; then
  USER_CFG="$HOME/.config/${APPNAME}"
  USER_SHARE="$HOME/.local/share/${APPNAME}"
  if [ -d "${USER_CFG}" ]; then
    log "Suppression des données utilisateur ${USER_CFG}"
    rm -rf "${USER_CFG}"
  else
    log "Aucun dossier de config utilisateur ${USER_CFG}"
  fi
  if [ -d "${USER_SHARE}" ]; then
    log "Suppression du contenu utilisateur ${USER_SHARE}"
    rm -rf "${USER_SHARE}"
  else
    log "Aucun dossier ~/.local/share/${APPNAME}"
  fi
fi

log "Désinstallation terminée."
echo "Désinstallation terminée. Voir ${LOGFILE} pour les détails."
