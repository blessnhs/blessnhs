#include "stdafx.h"

#include "./Command/Command.h"
#include "Constructor/Constructor.h"

int GetGoogleVerifyPurchase()
{
	

	// make uri
	/*http_client client(U("https://oauth2.googleapis.com/token"));
	uri_builder builder(U("/tokeninfo"));
	builder.append_query(U("id_token"),
		token.c_str());

	auto requestTask = client.request(methods::GET, builder.to_string());
	requestTask.wait();

	http_response response = requestTask.get();
	if (response.status_code() == status_codes::OK)
	{
	}*/



	try
	{
		//// make uri


		/*
		http_client client(L"https://fantasy.premierleague.com/drf/my-team/1463628/");
// Manually build up an HTTP request with header and request URI.
http_request request(methods::GET);
request.headers().add(L"Authorization", L"Basic am9obi5kb2VAZ21haWwuY29tOmFiYzEyMw==");

pplx::task<http_response> responses = client.request(request);
		*/

		/*
		http_client_config config;
    credentials creds( "username", "password" );
    config.set_credentials( creds );
    http_client client( U( "https://api.prosper.com/" ), config );*/


		http_client_config config;
		credentials cred(U("Authorization"), U("1//0elIP1kFzhj55CgYIARAAGA4SNwF-L9IrosFXYSA30SwFaMADpy-jrgipUF4x7JJWXTsGB7BQDVr4-y2DywLRYjsKKzZJOQOGnO8"));
		config.set_credentials(cred);
		
	/*	http_request request(methods::GET);
		request.headers().add(L"Authorization", L"Bearer eyJhbGciOiJSUzI1NiIsImtpZCI6ImQzN2FhNTA0MzgxMjkzN2ZlNDM5NjBjYTNjZjBlMjI4NGI2ZmMzNGQiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJodHRwczovL2FjY291bnRzLmdvb2dsZS5jb20iLCJhenAiOiI5MjY4NTA0Mjk5NDMtaTdsYmhmMGQ2amRsbG5sN2dhOGI3bHZxdTEyajJjMWQuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJhdWQiOiI5MjY4NTA0Mjk5NDMtZW52dXU0Z2E5aTEzM21iYXE1aGQ3N2cxYjliZGNyajUuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJzdWIiOiIxMTU1ODM4MzMxOTczMjY4NTE5NjYiLCJlbWFpbCI6ImJsZXNzbmhzNEBnbWFpbC5jb20iLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwibmFtZSI6IuuCmO2YhOyImCIsInBpY3R1cmUiOiJodHRwczovL2xoMy5nb29nbGV1c2VyY29udGVudC5jb20vYS9BRWRGVHA2TlMwQm1kakY5MndwdUFocm1ackIwa05oR1gtX1RYa2tvWm5IZz1zOTYtYyIsImdpdmVuX25hbWUiOiLtmITsiJgiLCJmYW1pbHlfbmFtZSI6IuuCmCIsImxvY2FsZSI6ImtvIiwiaWF0IjoxNjc0MTkzNTg3LCJleHAiOjE2NzQxOTcxODd9.AfpsBVdievz3TFG7HYIAwZeEcBRD-UHovnWRykbbGqh9Ihtvks1kdYBQR1g1w-2sZBJd75p9jQuEdwXiG6DJ5dV1YHNfoSkxocnZjg-A50IqgETCoIvBlAUAEBeAFj1GXr3RsBOqdPS44Bf-gvDS7P7r-f9CeWWWYkgT8QhIMKu8lnrpnzt07WBMaIblGagrskrsOE0lNrRN8d_rarYvgFluJ-pFFh0-b0QQv94f0_Zv12h3PRZ5FTknLdMT1zfpsrZ1KRFgl2RpbiL0Vx6QRuuVx_RJcpdD6YfT8UmG2eB2y1hRljQ-ok-XDpJuBWYEkcvBsD5njlLIke4X6fvlHA");*/

		http_client client(U("https://androidpublisher.googleapis.com/androidpublisher/v3/applications/com.blessnhs.cca/purchases/subscriptions/GPA.3359-8263-1411-31932/tokens/pnbkgecmfdlkpiklmfapgbki.AO-J1Ox7-yYC_dYjUJ8LbyFwwXAaUSsSrlLXc1kvqY24OGoDyHE35B9rE-tHqabsafrlwKIyC6csaXvpuwcs0EJKw7sUmIJBDQ"), config);

		auto requestTask = client.request(methods::GET);
		requestTask.wait();


		http_response response = requestTask.get();
		if (response.status_code() == status_codes::OK)
		{
			auto V = response.extract_json().get();

			//web::json::value _iss = V[U("iss")];
			//if (_iss.is_string())
			//{
			//	http_out.push_back(_iss.as_string());
			//}

			//web::json::value _sub = V[U("sub")];
			//if (_sub.is_string())
			//{
			//	http_out.push_back(_sub.as_string());
			//}

			//web::json::value _email = V[U("email")];
			//if (_email.is_string())
			//{
			//	http_out.push_back(_email.as_string());
			//}

			//web::json::value _name = V[U("name")];
			//if (_name.is_string())
			//{
			//	http_out.push_back(_T(_name.as_string()));
			//}

			//web::json::value _picture = V[U("picture")];
			//if (_picture.is_string())
			//{
			//	http_out.push_back(_picture.as_string());
			//}
			//web::json::value _locale = V[U("locale")];
			//if (_locale.is_string())
			//{
			//	http_out.push_back(_locale.as_string());
			//}
		}

		return 0;

	}
	catch (const std::exception& e)
	{
		printf("Error exception:%s\n", e.what());
		return -1;
	}

	return 0;
}


int main()
{
	GetGoogleVerifyPurchase();

	HubCommand _Command;
	HubConstructor			_Constructor;

	return 0;

}
