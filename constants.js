// DB configuration
const dbHost = "localhost";
const dbPort = "3306";
const dbUser = "root";
const dbPass = "@12qwwq21";
// Use the 'desktop' database for IoT desktop storage
const dbName = "desktop";

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

// TIMER QUERIES
const insertTimerTime = 'INSERT INTO timer (minutes, seconds, date) VALUES (?, ?, NOW())';
const selectTimerData = 'SELECT * FROM timer';

// BUTTON QUERIES
const insertButtonState = 'INSERT INTO button (state, date) VALUES (?, NOW())';
const selectButtonData = 'SELECT * FROM button';

// TEMPERATURE QUERIES
const insertTemperatureData ='INSERT INTO temperatura (valor, estado) VALUES (?, ?)';
const selectTemperatureData = 'SELECT * FROM temperature';

module.exports= {
   dbHost,dbPort,dbUser,dbPass,dbName,serverPort, contextURL,api,
   postTimerData, getTimerData,
   postButtonData, getButtonData,
   postTemperatureData, getTemperatureData,
   insertTimerTime, selectTimerData,
   insertButtonState, selectButtonData,
   insertTemperatureData, selectTemperatureData
}