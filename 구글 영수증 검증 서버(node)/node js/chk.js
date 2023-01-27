'use strict';

const crypto = require('crypto');
const axios = require('axios');
const https = require("https");

// email별 access_token 캐시
const requestCache = {};
const requestCacheTime = {};

// JWT 토큰을 만들어 access_token을 획득한다.
// 획득한 access_token은 requestCacheTime 만료시 까지 재사용 한다.
// email : Google Developer API - Service account id
// scope : https://www.googleapis.com/auth/androidpublisher
// tokenURL : https://accounts.google.com/o/oauth2/token
// pem : <String> pem key - openssl pkcs12 -in xxx-xxx.p12 -out xxx.pem -nodes
function getAccessToken(email, scope, tokenURL, pem) {

    // Cluster global 캐싱
    if (requestCache[email] && requestCacheTime[email] > +new Date()) return requestCache[email];

    const iat = Math.floor(+new Date() / 1000); // unixtimestamp
    const exp = iat + (60 * 60); // JWT expire는 1시간
    const header = new Buffer(JSON.stringify({ alg: 'RS256', typ: 'JWT' })).toString('base64'); // 헤더 인코딩
    const claimset = new Buffer(JSON.stringify({ iss: email, scope, aud: tokenURL, exp, iat })).toString('base64'); // 클레임 인코딩
    const unsigned = `${header}.${claimset}`; // JWT body
    const signature = crypto.createSign('RSA-SHA256').update(unsigned).sign(pem, 'base64'); // JWT signing
    const assertion = `${unsigned}.${signature}`; // JWT Token 생성 완료
    const grant_type = 'urn:ietf:params:oauth:grant-type:jwt-bearer';

    requestCacheTime[email] = +new Date() + (30 * 1000); // 30초 타임아웃

    return requestCache[email] =  axios.post(tokenURL, { assertion, grant_type }).then(result => {

  if (!result.data.access_token) throw new Error(JSON.stringify(result.data));
  requestCacheTime[email] = +new Date() + (result.data.expires_in * 1000) - (60 * 5 * 1000);
  return result.data.access_token;
});
}

module.exports = {
    
    // 구글 영수증 조회
    checkReceipt: (productId, token,packageName,res) => getAccessToken(
      'pubsub@pc-api-7836911650284524444-981.iam.gserviceaccount.com', // 서비스 계정 이메일
      'https://www.googleapis.com/auth/androidpublisher', // scope
      'https://accounts.google.com/o/oauth2/token', // 토큰 URL
      require('fs').readFileSync(__dirname + '/api.pem') // 서비스 계정 PEM
    ).then(access_token => {


        const url = `https://androidpublisher.googleapis.com/androidpublisher/v3/applications/${packageName}/purchases/products/${productId}/tokens/${token}`;
        const headers = { Authorization: `Bearer ${access_token}` };

        return axios.get(url, { headers : { Authorization : `Bearer ${access_token}` } })
	.then(result => {

	res.setHeader('Content-Type', 'application/json');
    	res.end(JSON.stringify(result.data));

	return result;
        }).catch( error => {
   	res.setHeader('Content-Type', 'application/json');
    	res.end(JSON.stringify({'code' : error.code}));
       });
    })

};