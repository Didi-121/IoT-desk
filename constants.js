//Database Configuration
//.ENV file is used to store sensitive information like DB credentials
const dbHost = process.env.DB_HOST || 'localhost';
const dbUser = process.env.DB_USER || 'root';
const dbPass = process.env.DB_PASS || 'password';
const dbPort = process.env.DB_PORT || '3306';
const dbName = process.env.DB_NAME || 'iot_database';

//Server General Configuration
const serverPort = 3000 

const contextURL = '/iot'; //Project context
const api = '/api'; // API context

// Timer endpoints
const postTimerData = '/timer/data';
const getTimerData = '/timer/data';

// Button endpoints
const postButtonData = '/button/data';
const getButtonData = '/button/data';

//DH11 Temperature endpoint
const postTemperatureData = '/temperature/data';
const getTemperatureData = '/temperature/data';

// Light endpoints
const postLightData = '/light/data';
const getLightData = '/light/data';

// Distance endpoints
const postDistanceData = '/distance/data';
const getDistanceData = '/distance/data';

// TIMER QUERIES
const insertTimerTime = 'INSERT INTO timer (minutes, seconds, date) VALUES (?, ?, NOW())';
const selectTimerData = 'SELECT * FROM timer';

// BUTTON QUERIES
const insertButtonState = 'INSERT INTO button (state, date) VALUES (?, NOW())';
const selectButtonData = 'SELECT * FROM button';

// TEMPERATURE QUERIES
const insertTemperatureData = 'INSERT INTO temperatura (value, state) VALUES (?, ?)';
const selectTemperatureData = 'SELECT * FROM temperatura ORDER BY id DESC';

// LIGHT QUERIES
const insertLightValue = 'INSERT INTO light (value, state) VALUES (?, ?)';
const selectLightData = 'SELECT * FROM light ORDER BY id DESC';

// DISTANCE QUERIES
const insertDistanceValue = 'INSERT INTO distance (value, state) VALUES (?, ?)';
const selectDistanceData = 'SELECT * FROM distance ORDER BY id DESC';

module.exports= {
   dbHost,dbPort,dbUser,dbPass,dbName,serverPort, contextURL,api,
   postTimerData, getTimerData,
   postButtonData, getButtonData,
   postTemperatureData, getTemperatureData,
   postLightData, getLightData,
   postDistanceData, getDistanceData,
   insertTimerTime, selectTimerData,
   insertButtonState, selectButtonData,
   insertTemperatureData, selectTemperatureData,
   insertLightValue, selectLightData,
   insertDistanceValue, selectDistanceData
}
