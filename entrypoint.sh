#!/usr/bin/env bash
set -euo pipefail

# default env vars
if [ -z "$VNC_PASSWORD" ]; then
  echo "ERROR: VNC_PASSWORD environment variable is not set!"
  exit 1
fi
VNC_GEOMETRY="${VNC_GEOMETRY:-1280x800}"
VNC_DISPLAY="${VNC_DISPLAY:-:1}"
VNC_USER="${VNC_USER:-tikzituser}"
USER_HOME="/home/$VNC_USER"

# ensure user exists (should already in Dockerfile)
if ! id "$VNC_USER" >/dev/null 2>&1; then
  useradd -m -s /bin/bash "$VNC_USER"
fi

# create vnc password
mkdir -p "$USER_HOME/.vnc"
echo "$VNC_PASSWORD" | vncpasswd -f > "$USER_HOME/.vnc/passwd"
chown -R "$VNC_USER:$VNC_USER" "$USER_HOME/.vnc"
chmod 600 "$USER_HOME/.vnc/passwd"

# create xstartup to launch XFCE and tikzit in the X session
# create xstartup to launch XFCE and tikzit in the X session
cat > "$USER_HOME/.vnc/xstartup" <<'EOF'
#!/bin/sh
xrdb $HOME/.Xresources

# 1. Start TikZiT in the BACKGROUND with a delay so XFCE loads first
(sleep 3 && /usr/local/bin/tikzit &>/tmp/tikzit.log) &

# 2. Start XFCE in the FOREGROUND (Do NOT use '&')
# This keeps the container running.
exec startxfce4
EOF

chown "$VNC_USER:$VNC_USER" "$USER_HOME/.vnc/xstartup"
chmod +x "$USER_HOME/.vnc/xstartup"

rm -rf /tmp/.X11-unix /tmp/.X*-lock
mkdir -p /tmp/.X11-unix
chmod 1777 /tmp/.X11-unix

# ensure noVNC exists in /opt/noVNC
if [ ! -d /opt/noVNC ]; then
  # Use the pinned version to avoid the "init_logging" error
  git clone --depth 1 --branch v1.4.0 https://github.com/novnc/noVNC.git /opt/noVNC
  git clone --depth 1 https://github.com/novnc/websockify.git /opt/noVNC/utils/websockify
fi

# start supervisord (will start vncserver and novnc)
exec "$@"
