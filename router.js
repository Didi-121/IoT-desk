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

router.post(constants.contextURL + constants.api + constants.postTemperatureData, sensors.insertTemperatureData);
router.get(constants.contextURL + constants.api + constants.getTemperatureData, sensors.getTemperatureData);

// LIGHT
router.post(constants.contextURL + constants.api + constants.postLightData, sensors.insertLightData);
router.get(constants.contextURL + constants.api + constants.getLightData, sensors.getLightData);

// DISTANCE
router.post(constants.contextURL + constants.api + constants.postDistanceData, sensors.insertDistanceData);
router.get(constants.contextURL + constants.api + constants.getDistanceData, sensors.getDistanceData);

// export the router
module.exports = router;