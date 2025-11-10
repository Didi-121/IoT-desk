/**
 * Librería que permite manejar las conexiones con Bases de Datos MySQL.
 * 
 * Se crea la conexión usando las constantes de MySQL definidas en /constants.js
 * Se crea un pool de conexiones para optimizar el acceso a la base de datos.
 * 
 * Se brindan métodos para realizar consultas (select), inserciones (insert),
 * actualizaciones (update) y borrados (delete).
 * 
 * Se recomienda NO MODIFICAR este archivo a menos que se requiera una funcionalidad nueva.
 * 
 * En IoT se enfoca en SQL y no en el manejo de un lenguaje de alto nivel.
 *  
 * Ernesto Cantú
 * 05/26/2025
 *  
 * @module MySQLManager
 * @requires mysql2/promise
 */
const mysql = require('mysql2/promise')
const constants = require('../constants')

const HOST =constants.dbHost;
const PORT = constants.dbPort;
const USR = constants.dbUser;
const PASS = constants.dbPass;
const DB = constants.dbName;

let pool = mysql.createPool({
  host: HOST,//constants.dbHost,
  user: USR,//The OS has a env variable called USER, causing an error on DB login.
  port: PORT,//constants.dbPort,
  password: PASS,//constants.dbPass,
  database: DB,//constants.dbName,
  decimalNumbers:true,
  waitForConnections: true,
  connectionLimit: 10,
});;

// Try a quick check to ensure the pool can get a connection and the DB is reachable.
(async function checkConnection(){
  try{
    const conn = await pool.getConnection();
    try{
      // simple query to validate the connection
      await conn.query('SELECT 1');
      console.log(`MySQL: connected OK to database "${DB}" at ${HOST}:${PORT} as ${USR}`);
    } finally {
      conn.release();
    }
  }catch(err){
    console.error('MySQL: connection check failed:', err && err.message ? err.message : err);
  }
})();

/**
 * Class that responds with an object of the result of a database interaction.
 */
class QueryResult{

  constructor(status,rows, gen_id, changes ,err){
      this.status = status;
      this.rows = rows;
      this.gen_id = gen_id;
      this.changes = changes;
      this.err = err;
  }

  getStatus = () => { return this.status;}
  getRows = () => { return this.rows;}
  getGenId = () => { return this.gen_id;}
  getChanges = () => { return this.changes;}
  getErr = () => { return this.err;}
}


/**
 * Method that extracts Data without params from a database.
 * 
 * @param {String} query the query to extract data
 * @returns A Query Result object
 */
async function getData(query){
  try{
    console.log("Get Data");
    let conn = await pool.getConnection();
    const [data,fields] = await conn.query(query);
    conn.release();
    return new QueryResult(true,data,0,0,'');
  }catch(error){
    console.log(error);
    return new QueryResult(false,null,0,0,error);
  }
}


/**
 * This method executes a query with params
 * Perfect for search queries.
 * 
 * @param {String} query the search query
 * @param {Array} params array of params
 * @returns {Object} An object of the class QueryResult
 */
async function getDataWithParams(query,params){
  try{
      console.log("GetData");
      const conn = await pool.getConnection();
      const [data,fields] = await conn.query(query,params);
      conn.release();
      return new QueryResult(true,data,0,0,'');
  }catch(error){
    console.log(error);
    return new QueryResult(false,null,0,0,error);
  }
}

/**
 * Makes an insert of an object on the Database.
 * 
 * @param {String} query The insert query that will be executed
 * @param {Array} params the params for the insert
 * @returns {Object} An object of the class QueryResult
 */
async function insertData(query,params){
  let conn;
  try{
      console.log("Insert Data");
      conn = await pool.getConnection();
      await conn.beginTransaction();
      const [data,fields] = await conn.query(query,params);
      await conn.commit();
      return new QueryResult(true,data,data.insertId,data.affectedRows,'')
  }catch(error){
    console.log(error);
    if(conn)
      await conn.rollback();
    return new QueryResult(false,null,0,0,error); 
  } finally {
    if (conn) 
      conn.release();
  }
}

/**
 * Bulk insert method for inserting multiple rows on a database table.
 * 
 * @param {String} query The insert query that will be executed
 * @param {Array} params the params for the insert
 * @returns {Object} An object of the class QueryResult
 */
async function bulkInsertData(query,elements){
  let conn;
  try{
      console.log("BULK Insert Data");
      conn = await pool.getConnection();
      await conn.beginTransaction();
      const [data,fields] = await conn.query(query,[elements]);
      await conn.commit();
      conn.release();  
      return new QueryResult(true,data,data.insertId,data.affectedRows,'');
  } catch(error){
    console.log(error);
    if(conn)  
      try { await conn.rollback(); } catch (e) { console.error("Rollback failed:", e); }
    return new QueryResult(false,null,0,0,error); 
  } finally {
    if (conn)
      conn.release();
  }
}

/**
 * Method that executes a query update on the database.
 * It can be an update or delete.
 * 
 * @param {String} query the update query
 * @param {*} params the array of params
 * @returns a Query Result Object
 */
async function updateData(query,params){
  let conn;
  try{
      console.log("Update Data");
      conn = await pool.getConnection();
      await conn.beginTransaction();
      const [data,fields] = await conn.query(query,params);
      await conn.commit();
      return new QueryResult(true,data,0,data.affectedRows,'');
  }catch(error){
    console.log(error);
    if(conn) 
      await conn.rollback();
    return new QueryResult(false,null,0,0,error); 
  } finally{
    if(conn) conn.release();
  }
}

/**
 * Method that calls a stored procedure with the given query and params.
 * 
 * @param {String} query The stored procedure query
 * @param {Array} params The parameters for the stored procedure
 * @returns {Object} A Query Result object
 */
async function callProcedure(query, params) {
  let conn;
  try {
    conn = await pool.getConnection();
    const [rows] = await conn.query(query, params);
    return new QueryResult(true, rows, 0, 0, '');
  } catch (error) {
    console.error(error);
    return new QueryResult(false, null, 0, 0, error);
  } finally {
    if (conn) conn.release();
  }
}



  module.exports = {QueryResult,getData,getDataWithParams,insertData,bulkInsertData,updateData, callProcedure}