import express from 'express';
import expressWs from 'express-ws';
import { spawn } from 'child_process';
import path from 'path';
import { fileURLToPath } from 'url';
import { v4 as uuidv4 } from 'uuid';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
expressWs(app);

const PORT = process.env.PORT || 8080;
const DIST_IMG = path.join(__dirname, '..', '..', 'dist', 'minios.img');
const FRONTEND_DIR = path.join(__dirname, '..', '..', 'frontend');

app.use(express.static(FRONTEND_DIR));

// Download OS image
app.get('/download/minios.img', (req, res) => {
  res.setHeader('Content-Type', 'application/octet-stream');
  res.setHeader('Content-Disposition', 'attachment; filename="minios.img"');
  res.sendFile(DIST_IMG);
});

// Per-connection QEMU
app.ws('/ws', (ws, req) => {
  const id = uuidv4();
  const qemuArgs = [
    '-nographic',
    '-serial', 'stdio',
    '-drive', `format=raw,file=${DIST_IMG}`,
  ];
  const qemu = spawn('qemu-system-i386', qemuArgs, { stdio: ['pipe', 'pipe', 'pipe'] });

  const send = (data) => {
    if (ws.readyState === 1) ws.send(data);
  };

  qemu.stdout.on('data', (data) => send(data));
  qemu.stderr.on('data', (data) => send(data));

  qemu.on('close', (code) => {
    send(`\n[QEMU exited ${code}]\n`);
    try { ws.close(); } catch {}
  });

  ws.on('message', (msg) => {
    // msg should be string/Buffer from xterm; forward to QEMU stdin
    if (typeof msg === 'string') qemu.stdin.write(msg);
    else qemu.stdin.write(Buffer.from(msg));
  });

  ws.on('close', () => {
    try { qemu.stdin.end(); } catch {}
    try { qemu.kill('SIGTERM'); } catch {}
  });
});

app.listen(PORT, () => {
  console.log(`MiniOS server listening on :${PORT}`);
  console.log('Open http://localhost:' + PORT);
});

