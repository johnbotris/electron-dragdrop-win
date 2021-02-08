const addon = require('bindings')('electron-dragdrop-win')
const os = require('os');

let doDragDrop

if(os.platform() == 'win32') {
    doDragDrop = {
        doDragDrop: promisify(addon.DoDragDrop)
    }
} else {
    doDragDrop = {
        doDragDrop: () => Promise.reject(`doDragDrop is not supported on ${os.platform()}`)
    }    
}

module.exports = doDragDrop;

function promisify(functionTakesCallback) {
    return function() {
        return new Promise((resolve, reject) => {
            functionTakesCallback(...arguments, function(err, res) {
                if (err != null) reject(err)
                else resolve(res)
            })
        })
    }
}