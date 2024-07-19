#########################
#########################

CONFIG:
	PARSING:
	
	INTERPRETING:

########################
########################

EXEC:
	USE:
		> poll() not the other ones
		
	DO:
		> Have default error pages
		> Have the same behavior as NGINX Https 1.1
			> For header response
			> Implement [ GET, POST, DELETE ]			
			> Have Accurate HTTP Status codes
			
	USECASES:
		> Serve Static Website
		> Handle PHP through CGI forks()
		> Upload Files from a client

########################
########################

BONUSES:
	> Handle multiple CGI (Python, php-fpm ...)
	> Have Sessions cookies
		> Keep conections TOKENS in the client cookies
		> (IDK what else can cookies be in a webserver)
