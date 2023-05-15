declare var MQTTClient: {
    (options, scope): void;
    computeUrl(): string;
    on(event, handler): void;
    emit(event): void;
    connect(): void;
    disconnect(): void;
    publish(topic, payload): void;
    subscribe(topic): void;
};