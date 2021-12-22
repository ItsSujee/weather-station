FROM eclipse-mosquitto

COPY mosquitto.conf /mosquitto/config/mosquitto.conf

ARG USERNAME
ARG PASSWORD

RUN mosquitto_passwd -c -b passwd $USERNAME $PASSWORD

EXPOSE 1883

