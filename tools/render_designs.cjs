// Rasterize the vector design studies, not JUCE component screenshots.
const { Resvg } = require('@resvg/resvg-js');
const fs = require('fs');
const path = require('path');
const root = path.resolve(__dirname, '..');
const directory = path.join(root, 'Design', 'UI-Previews');
for (const file of fs.readdirSync(directory).filter(f => f.endsWith('.svg'))) {
    const renderer = new Resvg(fs.readFileSync(path.join(directory, file)), {
        font: {
            fontFiles: ['RajdhaniMedium.ttf', 'RajdhaniSemiBold.ttf'].map(f => path.join(root, 'Design', 'Fonts', f)),
            loadSystemFonts: false
        },
        fitTo: { mode: 'width', value: 2400 }
    });
    fs.writeFileSync(path.join(directory, file.replace('.svg', '.png')), renderer.render().asPng());
    console.log('Design export:', file);
}
