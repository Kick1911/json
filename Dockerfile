FROM alpine:latest
WORKDIR /code
RUN apk add --no-cache gcc musl-dev linux-headers make
COPY . .
CMD ["make", "package"]
