#!/usr/bin/env python3
"""
CGI de prueba para webserv (C++98 / epoll).
Pensado para forzar los puntos donde tu implementacion suele romperse:
- Lectura de stdin cuando CONTENT_LENGTH esta presente (POST)
- Lectura de stdin cuando el body viene chunked (sin CONTENT_LENGTH, solo Transfer-Encoding)
- Variables de entorno CGI obligatorias segun RFC 3875
- Salida con Content-Length correcto vs salida sin Content-Length (deberia cerrar conexion o usar chunked)
- Codigos de estado no-200 (via query string ?status=404)
- Un bucle que tarda (?sleep=N) para probar que tu epoll no bloquea otras conexiones mientras el CGI corre
"""

import os
import sys
import time

def read_stdin_body():
    """
    Si el servidor te pasa CONTENT_LENGTH, DEBES leer exactamente esos bytes.
    Leer de mas bloquea (pipe/fd sin EOF hasta que el padre cierre).
    Leer de menos deja el body a medias.
    Si no hay CONTENT_LENGTH (chunked de verdad decodificado antes de pasarlo al CGI,
    que es lo correcto: el CGI NUNCA deberia recibir chunked crudo), leemos hasta EOF.
    """
    length = os.environ.get("CONTENT_LENGTH")
    if length is not None and length.strip() != "":
        try:
            n = int(length)
        except ValueError:
            return b"", "CONTENT_LENGTH invalido (no numerico): %r" % length
        data = sys.stdin.buffer.read(n)
        if len(data) != n:
            return data, "AVISO: se esperaban %d bytes y se leyeron %d (fd cerrado antes de tiempo?)" % (n, len(data))
        return data, None
    else:
        # Sin content-length: si tu server pasa aqui el body sin decodificar el chunked,
        # esto es un bug de tu implementacion, no del CGI.
        data = sys.stdin.buffer.read()
        return data, "Sin CONTENT_LENGTH -> leido hasta EOF (%d bytes)" % len(data)


def parse_query(qs):
    params = {}
    if not qs:
        return params
    for pair in qs.split("&"):
        if not pair:
            continue
        if "=" in pair:
            k, v = pair.split("=", 1)
        else:
            k, v = pair, ""
        params.setdefault(k, []).append(v)
    return params


def main():
    method = os.environ.get("REQUEST_METHOD", "")
    qs = os.environ.get("QUERY_STRING", "")
    params = parse_query(qs)

    # ?sleep=N -> util para verificar que el epoll loop del padre no se congela
    if "sleep" in params:
        try:
            secs = float(params["sleep"][0])
            time.sleep(min(secs, 10))  # cap defensivo
        except ValueError:
            pass

    # ?status=NNN -> fuerza un status line no estandar para probar tu parseo de la respuesta del CGI
    status = "200 OK"
    if "status" in params:
        code = params["status"][0]
        reason = {
            "404": "Not Found",
            "500": "Internal Server Error",
            "204": "No Content",
        }.get(code, "Custom")
        status = "%s %s" % (code, reason)

    body_bytes, body_warning = (b"", None)
    if method in ("POST", "PUT", "PATCH"):
        body_bytes, body_warning = read_stdin_body()

    env_dump_keys = [
        "REQUEST_METHOD", "SCRIPT_NAME", "PATH_INFO", "QUERY_STRING",
        "CONTENT_TYPE", "CONTENT_LENGTH", "SERVER_PROTOCOL", "SERVER_NAME",
        "SERVER_PORT", "REMOTE_ADDR", "GATEWAY_INTERFACE", "REDIRECT_STATUS",
    ]

    def esc(v):
        if v is None:
            return "<span class=\"null\">NULL</span>"
        s = str(v)
        return (s.replace("&", "&amp;").replace("<", "&lt;")
                 .replace(">", "&gt;").replace('"', "&quot;"))

    status_code = status.split(" ", 1)[0]
    status_class = "s2xx"
    if status_code.startswith("4"):
        status_class = "s4xx"
    elif status_code.startswith("5"):
        status_class = "s5xx"
    elif status_code.startswith("3"):
        status_class = "s3xx"

    def kv_rows(pairs):
        return "".join(
            "<tr><td class=\"k\">%s</td><td class=\"v\">%s</td></tr>" % (esc(k), esc(v))
            for k, v in pairs
        )

    env_rows = kv_rows((k, os.environ.get(k)) for k in env_dump_keys)
    http_rows = kv_rows(
        (k, v) for k, v in sorted(os.environ.items()) if k.startswith("HTTP_")
    ) or "<tr><td colspan=\"2\" class=\"empty\">Ningun header HTTP_* recibido</td></tr>"

    query_rows = kv_rows((k, ", ".join(v)) for k, v in params.items()) or \
        "<tr><td colspan=\"2\" class=\"empty\">Sin query string</td></tr>"

    try:
        preview = body_bytes.decode("utf-8", errors="replace")
    except Exception:
        preview = "<binario>"

    body_section = ""
    if method in ("POST", "PUT", "PATCH"):
        warning_html = ("<div class=\"warn\">%s</div>" % esc(body_warning)) if body_warning else ""
        body_section = """
        <section class="card">
          <h2>Body <span class="badge">%d bytes</span></h2>
          %s
          <pre class="body-preview">%s</pre>
        </section>""" % (len(body_bytes), warning_html, esc(preview[:500]))

    body_out = ("""<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<title>webserv CGI test</title>
<style>
  :root {
    --bg: #0d1117; --card: #161b22; --border: #30363d;
    --text: #c9d1d9; --muted: #8b949e; --accent: #58a6ff;
  }
  * { box-sizing: border-box; }
  body {
    background: var(--bg); color: var(--text);
    font-family: -apple-system, "Segoe UI", Roboto, sans-serif;
    margin: 0; padding: 32px 16px;
  }
  .wrap { max-width: 900px; margin: 0 auto; }
  header {
    display: flex; align-items: center; gap: 12px;
    margin-bottom: 24px;
  }
  header h1 { font-size: 20px; margin: 0; font-weight: 600; }
  .pid { color: var(--muted); font-size: 13px; font-family: monospace; }
  .status-pill {
    display: inline-block; padding: 4px 10px; border-radius: 999px;
    font-size: 12px; font-weight: 700; font-family: monospace;
  }
  .s2xx { background: #1a4d2e; color: #56d364; }
  .s3xx { background: #4d3d1a; color: #d3b656; }
  .s4xx { background: #4d2a1a; color: #d38556; }
  .s5xx { background: #4d1a1a; color: #d35656; }
  .card {
    background: var(--card); border: 1px solid var(--border);
    border-radius: 10px; padding: 18px 20px; margin-bottom: 16px;
  }
  .card h2 {
    font-size: 13px; text-transform: uppercase; letter-spacing: .06em;
    color: var(--muted); margin: 0 0 12px 0; font-weight: 600;
    display: flex; align-items: center; gap: 8px;
  }
  .badge {
    background: #21262d; color: var(--accent); padding: 2px 8px;
    border-radius: 6px; font-size: 11px; font-family: monospace;
  }
  table { width: 100%%; border-collapse: collapse; font-family: monospace; font-size: 13px; }
  td { padding: 5px 8px; border-bottom: 1px solid var(--border); vertical-align: top; }
  td.k { color: var(--accent); width: 220px; white-space: nowrap; }
  td.v { color: var(--text); word-break: break-all; }
  td.empty { color: var(--muted); font-style: italic; }
  .null { color: var(--muted); font-style: italic; }
  .warn {
    background: #4d3d1a; color: #d3b656; padding: 8px 12px;
    border-radius: 6px; font-family: monospace; font-size: 12px; margin-bottom: 10px;
  }
  .body-preview {
    background: #0d1117; border: 1px solid var(--border); border-radius: 6px;
    padding: 12px; font-size: 12px; overflow-x: auto; white-space: pre-wrap;
    word-break: break-all; margin: 0;
  }
</style>
</head>
<body>
<div class="wrap">
  <header>
    <h1>CGI PYTHON</h1>
    <span class="status-pill %s">%s</span>
    <span class="pid">pid %d</span>
  </header>

  <section class="card">
    <h2>Variables CGI estandar</h2>
    <table>%s</table>
  </section>

  <section class="card">
    <h2>Headers HTTP_*</h2>
    <table>%s</table>
  </section>

  <section class="card">
    <h2>Query string</h2>
    <table>%s</table>
  </section>
  %s
</div>
</body>
</html>""" % (status_class, esc(status), os.getpid(), env_rows, http_rows, query_rows, body_section)).encode("utf-8")

    # Cabecera CGI: status + content-type + content-length + linea en blanco obligatoria.
    # Si tu server no respeta que el CGI puede omitir Content-Length, esa es otra cosa a probar
    # (comenta la linea de abajo para forzar ese caso).
    sys.stdout.buffer.write(b"Status: %s\r\n" % status.encode())
    sys.stdout.buffer.write(b"Content-Type: text/html\r\n")
    sys.stdout.buffer.write(b"Content-Length: %d\r\n" % len(body_out))
    sys.stdout.buffer.write(b"\r\n")
    sys.stdout.buffer.write(body_out)
    sys.stdout.buffer.flush()


if __name__ == "__main__":
    main()
