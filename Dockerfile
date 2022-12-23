FROM nginx

# Install NGINX
RUN apt-get update && apt-get install -y nginx

# Expose ports 4280 and 42443
EXPOSE 4280 42443

# Copy the configuration file to the NGINX configuration directory
COPY webserv.conf /etc/nginx/conf.d/webserv.conf

# Start NGINX
CMD ["nginx", "-g", "daemon off;"]

