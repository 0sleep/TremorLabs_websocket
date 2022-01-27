const path = require('path');

module.exports = {
  mode: 'development',
  entry: './websocketclient_html.js',
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'main.bundle.js',
  },
};
