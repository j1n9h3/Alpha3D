// Time.h
class Time {
public:
    static void Update();
    static float DeltaTime() { return delta_time; }
private:
    static float delta_time;
    static double last_time;
};