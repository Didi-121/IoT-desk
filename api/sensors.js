const mysql = require("../database/MySQLMngr");
const constants = require("../constants");

const STATE_MAP = [ "SET_MINUTES", "SET_SECONDS", "RUNNING", "PAUSED", "FINISHED" ];

//Endpoint 1: recibe datos del temporizador
async function insertTimeData(req, res) {
    try {
        const serverTime = new Date().toISOString();
        console.log(`[TIME_DATA] Received at ${serverTime}`);
        console.log(' Payload:', req.body);

        // Validación básica
        if (req.body.minutes === undefined || req.body.seconds === undefined) {
             return res.status(400).json({ error: "Missing minutes or seconds" });
        }

        let query = constants.insertTimerTime; 
        let params = [req.body.minutes, req.body.seconds];

        const qResult = await mysql.insertData(query, params);
        res.status(200).json(qResult);

    } catch (error) {
        console.error(error);
        res.status(500).json({ status: "error", message: error.message });
    }
}

//Endpoint 2: recibe estados del boton 
async function insertButtonData(req, res) {
    try {
        const serverTime = new Date().toISOString();
        console.log(`[BUTTON_DATA] Received at ${serverTime}`);
        console.log(' Payload:', req.body);

        if (req.body.state === undefined) {
            return res.status(400).json({ error: "Missing state" });
        }

        // Convertir int a string
        let stateString = STATE_MAP[req.body.state] || "UNKNOWN";
        
        let query = constants.insertButtonState;
        let params = [stateString];

        const qResult = await mysql.insertData(query, params);
        res.status(200).json(qResult);

    } catch (error) {
        console.error(error);
        res.status(500).json({ status: "error", message: error.message });
    }
}

//Endpoint 3: obtiene datos del temporizador
async function getTimerData(req, res) {
    try {
        const serverTime = new Date().toISOString();
        console.log(`[GET_TIMER_DATA] Requested at ${serverTime}`);

        let query = constants.selectTimerData;
        const qResult = await mysql.queryData(query, []);
        res.status(200).json(qResult);

    } catch (error) {
        console.error(error);
        res.status(500).json({ status: "error", message: error.message });
    }
}

//Endpoint 4: obtiene datos del botón
async function getButtonData(req, res) {
    try {
        const serverTime = new Date().toISOString();
        console.log(`[GET_BUTTON_DATA] Requested at ${serverTime}`);

        let query = constants.selectButtonData;
        const qResult = await mysql.queryData(query, []);
        res.status(200).json(qResult);

    } catch (error) {
        console.error(error);
        res.status(500).json({ status: "error", message: error.message });
    }
}

//Endpoint 5: recibe datos del DH11
async function insertTemperatureData(req, res){
  try{
    const serverTime = new Date().toISOString();
    console.log(`[TEMPERATURE_DATA] Received at ${serverTime}`);
    console.log('Payload:', req.body);

    if (!req.body || req.body.temperature === undefined) {
      return res.status(400).json({ error: "Missing temperature" });
    }

    const temperature = req.body.temperature;
    const estado = req.body.estado === true ? 1 : 0;

    const query = constants.insertTemperatureData;
    const params = [temperature, estado];  

    const qResult = await mysql.insertData(query, params);
    return res.status(200).json(qResult);

  } catch (error) {
    console.error(error);
    return res.status(500).json({status: "error", message: error.message});
  }
}

//Endpoint 6: obtiene datos del DH11
async function getTemperatureData(req, res){
    try{
        const serverTime = new Date().toISOString();
        console.log(`[GET_TEMPERATURE_DATA] Requested at ${serverTime}`);

        let query = constants.selectTemperatureData;
        const qResult = await mysql.queryData(query, []);
        res.status(200).json(qResult);
    } catch (error) {
        console.error(error);
        res.status(500).json({status: "error", message: error.message});
    }
}

module.exports = { insertTimeData, insertButtonData, getTimerData, getButtonData, insertTemperatureData, getTemperatureData };