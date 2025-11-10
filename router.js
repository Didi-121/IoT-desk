//Script to configure all the routes of the project
//Dependencies 
const express = require('express');
const router = express.Router();
const constants =  require('./constants');
const sensors = require('./api/sensors'); // file for light sensor requests

// endpoint 
router.post(constants.contextURL + constants.api + constants.postLightSensor, sensors.insertNewLightLevel);

// export the router
module.exports = router;