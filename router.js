//Script to configure all the routes of the project
//Dependencies 
const express = require('express');
const router = express.Router();
const constants =  require('./constants');
const sensors = require('./api/sensors'); // file for light sensor requests

// endpoint 
router.post(constants.contextURL + constants.api + constants.postTimerData, sensors.insertTimeData);
router.get(constants.contextURL + constants.api + constants.getTimerData, sensors.getTimerData);

router.post(constants.contextURL + constants.api + constants.postButtonData, sensors.insertButtonData);
router.get(constants.contextURL + constants.api + constants.getButtonData, sensors.getButtonData);

// export the router
module.exports = router;