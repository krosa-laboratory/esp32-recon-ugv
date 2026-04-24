#ifndef WEBPAGE_H
#define WEBPAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
    <head>
        <title>Recon Scout HUD</title>
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
        <style>
            body { background-color: #111; color: #0f0; font-family: monospace; text-align: center; margin: 0; user-select: none; }
            #video-stream { width: 100%; height: 200px; background: #222; border-bottom: 2px solid #444; line-height: 200px;}
            .controls { display: flex; justify-content: space-between; height: 50vh; padding: 20px; }
            .joystick-zone { width: 40%; background: rgba(0, 255, 0, 0.1); border: 1px dashed #0f0; border-radius: 10px; display: flex; align-items: center; justify-content: center; position: relative; touch-action: none; }
            .thumb { width: 50px; height: 50px; background: #0f0; border-radius: 50%; position: absolute; opacity: 0.5; pointer-events: none; top: calc(50% - 25px); }
        </style>
    </head>
    <body>
        <div id="video-stream">STREAMING AREA</div>
        <h2>STATUS: <span id="status" style="color:red;">OFFLINE</span></h2>
        
        <div class="controls">
            <div id="joy-left" class="joystick-zone"><div id="thumb-l" class="thumb"></div></div>
            <div id="joy-right" class="joystick-zone"><div id="thumb-r" class="thumb"></div></div>
        </div>

        <script>
            // Connect automatically to port 81 of ESP32
            var ws = new WebSocket(`ws://${window.location.hostname}:81/`);
            ws.onopen = () => { document.getElementById('status').innerText = 'ONLINE'; document.getElementById('status').style.color = '#0f0'; };
            ws.onclose = () => { document.getElementById('status').innerText = 'OFFLINE'; document.getElementById('status').style.color = 'red'; };

            let left_speed = 0, right_speed = 0;

            // Send speed values 20 times per second
            setInterval(() => {
                if(ws.readyState === WebSocket.OPEN) {
                    ws.send(`${left_speed},${right_speed}`);
                }
            }, 50);

            // Maths for tactil joysticks
            function handleTouch(e, zone_id, thumb_id, is_left) {
            e.preventDefault();
            let touch = e.targetTouches[0];
            let zone = document.getElementById(zone_id);
            let thumb = document.getElementById(thumb_id);
            
            if (e.type === 'touchstart' || e.type === 'touchmove') {
                let rect = zone.getBoundingClientRect();
                let y = touch.clientY - rect.top;
                y = Math.max(0, Math.min(y, rect.height)); 
                thumb.style.top = (y - 25) + 'px'; 
                // Conver Y position into speed from -100 to 100
                let speed = Math.round(((rect.height / 2) - y) / (rect.height / 2) * 100);
                if(is_left) left_speed = speed; else right_speed = speed;
            } else if (e.type === 'touchend') {
                // When the touch end, automatically reduce speed to 0
                thumb.style.top = 'calc(50% - 25px)';
                if(is_left) left_speed = 0; else right_speed = 0;
            }
            }

            ['touchstart', 'touchmove', 'touchend'].forEach(evt => {
                document.getElementById('joy-left').addEventListener(evt, (e) => handleTouch(e, 'joy-left', 'thumb-l', true));
                document.getElementById('joy-right').addEventListener(evt, (e) => handleTouch(e, 'joy-right', 'thumb-r', false));
            });
        </script>
    </body>
</html>
)rawliteral";

#endif
