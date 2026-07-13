<?php
/**
 * CGI de prueba en PHP para webserv.
 * IMPORTANTE: esto se ejecuta con `php-cgi`, NO con el `php` del CLI normal
 * ni con el servidor embebido (`php -S`). Si tu execve() apunta al binario
 * equivocado, esto falla en silencio o devuelve texto plano sin parsear cabeceras.
 *
 * Verifica primero en tu maquina:
 *   which php-cgi
 * y que tu config asocia la extension .php a esa ruta, no a /usr/bin/php.
 */

// PHP con display_errors a stdout puede mezclar warnings con el header block
// y romper el parseo de tu servidor. Lo forzamos a log en vez de stdout.
ini_set('display_errors', '0');
ini_set('log_errors', '1');

function read_stdin_body(): array {
    $length = getenv('CONTENT_LENGTH');
    if ($length !== false && $length !== '') {
        $n = (int)$length;
        $data = '';
        $remaining = $n;
        // Lectura en bucle: fread puede devolver menos de lo pedido en una sola llamada.
        while ($remaining > 0) {
            $chunk = fread(STDIN, $remaining);
            if ($chunk === false || $chunk === '') {
                break; // EOF antes de tiempo -> lo reportamos, no colgamos el script
            }
            $data .= $chunk;
            $remaining -= strlen($chunk);
        }
        if (strlen($data) !== $n) {
            return [$data, "AVISO: se esperaban $n bytes y se leyeron " . strlen($data) . " (fd cerrado antes de tiempo?)"];
        }
        return [$data, null];
    }
    $data = stream_get_contents(STDIN);
    return [$data, 'Sin CONTENT_LENGTH -> leido hasta EOF (' . strlen($data) . ' bytes)'];
}

$method = getenv('REQUEST_METHOD') ?: '';
$qs = getenv('QUERY_STRING') ?: '';
parse_str($qs, $params);

if (isset($params['sleep'])) {
    $secs = min((float)$params['sleep'], 10);
    usleep((int)($secs * 1000000));
}

$status = '200 OK';
if (isset($params['status'])) {
    $code = $params['status'];
    $reasons = ['404' => 'Not Found', '500' => 'Internal Server Error', '204' => 'No Content'];
    $reason = $reasons[$code] ?? 'Custom';
    $status = "$code $reason";
}

$body = '';
$warning = null;
if (in_array($method, ['POST', 'PUT', 'PATCH'], true)) {
    [$body, $warning] = read_stdin_body();
}

$envKeys = [
    'REQUEST_METHOD', 'SCRIPT_NAME', 'PATH_INFO', 'QUERY_STRING',
    'CONTENT_TYPE', 'CONTENT_LENGTH', 'SERVER_PROTOCOL', 'SERVER_NAME',
    'SERVER_PORT', 'REMOTE_ADDR', 'GATEWAY_INTERFACE', 'REDIRECT_STATUS',
];

$out = "<html><body><pre>\n";
$out .= "=== CGI PHP OK ===\n";
$out .= "pid: " . getmypid() . "\n\n";

$out .= "--- Variables CGI estandar ---\n";
foreach ($envKeys as $k) {
    $v = getenv($k);
    $out .= sprintf("%-18s = %s\n", $k, $v === false ? 'NULL' : var_export($v, true));
}

$out .= "\n--- Headers HTTP_* recibidos ---\n";
foreach ($_SERVER as $k => $v) {
    if (strpos($k, 'HTTP_') === 0) {
        $out .= sprintf("%-25s = %s\n", $k, var_export($v, true));
    }
}

$out .= "\n--- Query string parseada ---\n";
$out .= var_export($params, true) . "\n";

$out .= "\n--- Body ---\n";
if ($warning) {
    $out .= "AVISO: $warning\n";
}
$out .= "bytes recibidos: " . strlen($body) . "\n";
$out .= "preview (primeros 500 chars): " . substr($body, 0, 500) . "\n";

$out .= "</pre></body></html>";

// Cabecera CGI manual. Nada de header() de PHP normal porque eso asume
// contexto SAPI web con su propio manejo; en modo CGI puro escribimos crudo.
echo "Status: $status\r\n";
echo "Content-Type: text/html\r\n";
echo "Content-Length: " . strlen($out) . "\r\n";
echo "\r\n";
echo $out;
