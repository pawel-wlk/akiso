# read chuck norris joke
curl http://api.icndb.com/jokes/random -s | jq '.value.joke'

#cat
#

curl 'https://api.thecatapi.com/v1/images/search' -s --header 'x-api-key: 492750b6-604f-4455-9190-9b9df4b92864' | jq '.[0].url' | xargs wget -q -O /tmp/cat
img2txt /tmp/cat -f utf8
rm /tmp/cat


