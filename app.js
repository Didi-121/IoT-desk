//Dependencies 
require('dotenv').config();
const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const router = require('./router')

// server created with express
const app = express();
const port = 3000;

// Cors is used for enabling Cross-Origin Resource Sharing
// Body-parser is used to parse the incoming requests as a json 
app.use(cors());

app.use(bodyParser.urlencoded({ extended: false }));
// Capture raw JSON body for logging and better error messages. The `verify`
// option gives us access to the raw buffer before body-parser attempts to
// parse it. We store it on `req.rawBody` so error handlers can log it.
app.use(bodyParser.json({
    verify: function (req, res, buf, encoding) {
        try {
            req.rawBody = buf.toString(encoding || 'utf8');
        } catch (e) {
            req.rawBody = '';
        }
    }
}));

// Log raw JSON body (only when content-type is application/json)
app.use((req, res, next) => {
    const ct = req.headers['content-type'] || '';
    if (ct.includes('application/json')) {
        console.log('--> Received raw JSON body:', req.rawBody);
    }
    next();
});

// Endpoints routes, see /routes/route.js
app.use(router);

// Endpoint - landing 
app.get('/', (req, res) => {
    res.send('Hello world!')
});

//Start the server
app.listen(port, () => {
    console.log('Server started running on : ' + port)
})