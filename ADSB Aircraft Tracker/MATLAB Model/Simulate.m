%define simulator parameters
simTime = 15; %time to simulate in seconds
simStep = 0.1; %time step size of simulation

%initial parameters
EZY123 = Aircraft(300,1000,304.8,0,0,0); % create aircraft
tracker = Tracker(0,0,0,0, 0); %create tracker object
EZY123.yvel = -128.611; % set aircraft velocity
EZY123.xvel = 0; % set aircraft velocity
EZY123.zvel = 0; % set aircraft velocity

%start with tracker looking at the aircraft
tracker.calculate(EZY123);
tracker.point;

%setup arrays for recording simulation results
tiltData = zeros(1,(simTime/simStep));
panData = zeros(1,(simTime/simStep));
desTiltRateData = zeros(1,(simTime/simStep));
desPanRateData = zeros(1,(simTime/simStep));
desPanAccelData = zeros(1,(simTime/simStep));
desTiltAccelData = zeros(1,(simTime/simStep));


%create figures to plot on
clear figure;
f = figure(1);

%step through the simulation
for i = 1:(simTime/simStep)
    EZY123.update(simStep); %updates aircraft
    tracker.calculate(EZY123);
    tracker.update(simStep); %updates tracker
    plotter(tracker, EZY123);
    desTiltRateData(i) = tracker.tiltVel;
    desPanRateData(i) = tracker.panVel;
    desPanAccelData(i) = tracker.panAccel;
    desTiltAccelData(i) = tracker.tiltAccel;
    tiltData(i) = tracker.tilt;
    panData(i) = tracker.pan;
end


%setup figures
figure(2)
subplot(3,2,1)
plot(tiltData)
title('Tilt Angle')
xlabel('time (s)')
ylabel('degrees')

subplot(3,2,3)
plot(desTiltRateData)
title('Tilt Velocity')
xlabel('time (s)')
ylabel('degrees/s')

subplot(3,2,5)
plot(desTiltAccelData)
title('Tilt Acceleration')
xlabel('time (s)')
ylabel('degrees/s^2')

subplot(3,2,2)
plot(panData)
title('Pan Angle')
xlabel('time (s)')
ylabel('degrees')

subplot(3,2,4)
plot(desPanRateData)
title('Pan Velocity')
xlabel('time (s)')
ylabel('degrees/s')

subplot(3,2,6)
plot(desPanAccelData)
title('Pan Acceleration')
xlabel('time (s)')
ylabel('degrees/s^2')



function plotter(tracker , aircraft)
    figure(1)
    clf
    set(axes, 'Ydir', 'reverse');
    %get tracker view in tracker frame (same as global frame when tracker position is 0 0 0 )
    p = tracker.view();
    p = p.';
    vertex= [p];
    face= [1 2 3 nan; 1 2 4 nan; 1 4 5  nan; 1 5 3 nan;  4 2 3 5];
    patch('Faces',face,'Vertices',vertex,'Facecolor',[0.8 0.8 1],'FaceAlpha', [0.5]);
    hold on
    plot3(tracker.x,tracker.y,tracker.z,'o','MarkerSize', 10);
    c130(aircraft.x, aircraft.y, aircraft.z)
    set(gca,'color',[1 1 1])
    xlim([-1000 1000])
    ylim([-1000 1000])
    zlim([0 1000])
    xlabel('X (m)')
    ylabel('Y (m)')
    zlabel('Z (m)')
    view(3);
    daspect([1 1 1]);
    grid on
    hold off
end




