# Amazon-Discount-Checker
TCP web scraper that will search Amazon to see if a product has a discount, if so then it will smtp the information

This is a personal project that is designed to help improve my knowledge on HTTP/HTTPS webscraping. Using C to handle all the httping.
The program uses OpenSSL to handle the encryption for HTTPS reguests as Amazon strictly uses HTTPS for every page no matter how sensitive.
At the moment the project can communicate with an Amazon page and gather the data and probe it to find out if its on sale or not, but it
cannot send out an smtp yet as im still working on how i want to present the data when it has all arrived.
