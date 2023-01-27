const chk = require('./chk.js');
const http = require("http");
const router= require("router");
const express = require("express");

const app = express();

////http://127.0.0.1?token=_token&purchase_name=_purchase_name&package_name=_package_name
app.get('/', function (req, res, next) {

    var token = req.query.token;
    var purchase_name = req.query.purchase_name;
    var package_name = req.query.package_name;

    console.log(token);
    console.log(purchase_name);
    console.log(package_name);

    chk.checkReceipt(purchase_name,token,package_name,res);

  
});

app.listen(8080, () => {});
