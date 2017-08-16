const dragDrop = require('bindings')('electron-dragdrop-win')
const os = require('os');

var dragDropExport;
if(os.platform() == 'win') {
    dragDropExport = {
        doDragDrop: dragDrop.DragDrop
    }
} else {
    dragDropExport = {
        doDragDrop: function() {}
    }    
}

module.exports = dragDropExport;
