const { app, BrowserWindow, ipcMain } = require('electron')
const path = require('path')
const dragDrop = require('../index');

function startup() {
    const win = new BrowserWindow({
        width: 400,
        height: 400,
        webPreferences: {
            nodeIntegration: true,
        }
    })
    win.loadFile('./index.html')
    win.webContents.openDevTools()



    ipcMain.handle('drag', (event, ...args) => {
        console.log("dragging start")
        // const file = "./1280px-Slovenia_location_map.svg.png"
        // const icon = nativeImage.createFromPath("./icon.png")
        // var cfTextFormat = dragDrop.createStandardAsciiTextFormat("Ashkan Daie");
        // var cfUnicodeTextFormat = dragDrop.createStandardUnicodeTextFormat("Ashkan Daie");
        // var filename = path.join(__dirname, 'obama.png');
        // var imageBuffer = fs.readFileSync(filename);
        // var cfBitmapFormat = dragDrop.createStandardBitmapFormat(imageBuffer);
        // console.log(cfBitmapFormat.format, cfTextFormat.data.length);

        // const fetched = fetch("http://192.168.178.35:9000/iconsmaller.png")
        //     .then(response => {
        //         console.log("response", response)
        //     })
        //     .catch(err => console.log("err", err))

        const filename = path.join(__dirname, 'file.xml');
        const data = "<xml>Sample Drag and Drop Payload</xml>";
        const ascii = Buffer.from(data, 'ascii');
        const utf8 = Buffer.from(data, 'utf8');
        const unicode = Buffer.from(data, 'utf16le');

        const options = {
            windowHandle: win.getNativeWindowHandle(),
            formats: {
                text: data,
                CF_ASCII: ascii,
                CF_UTF8: utf8,
                CF_UNICODE: unicode
            },
            events: {
                completed: (data) => console.log('complete', data),
                dragEnter: (data) => console.log('dragEnter', data),
                dragLeave: () => console.log('dragLeave')
            }
        };

        log("sadlfkja;s2")

        dragDrop.doDragDrop(options);
        return "ok"
    })

    function log(...args) {
        win.webContents.send("log", "<main>", ...args)
    }

}

app.whenReady().then(startup)
app.on('activate', () => !win && startup())

app.on('window-all-closed', () => {
    app.quit()
})


