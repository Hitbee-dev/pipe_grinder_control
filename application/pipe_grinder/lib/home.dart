import 'package:flutter/material.dart';
import 'package:flutter/src/foundation/key.dart';
import 'package:flutter/src/widgets/framework.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:fluttertoast/fluttertoast.dart';
import 'package:get/get.dart';
import 'package:pipe_grinder/bluetooth.dart';
import 'package:step_progress_indicator/step_progress_indicator.dart';

class Home extends StatefulWidget {
  const Home({Key? key}) : super(key: key);

  @override
  State<Home> createState() => _HomeState();
}

class _HomeState extends State<Home> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        iconTheme: IconThemeData(color: Colors.white),
        backgroundColor: Colors.black87, //appbar 투명색
        centerTitle: true,
        elevation: 2.0, // 그림자 농도 0
        leading: IconButton(
          icon: Icon(Icons.bluetooth),
          onPressed: () {
            print("블루투스");
            Get.toNamed("/bluetooth");
          },
        ),
        actions: [
          IconButton(
            icon: Icon(Icons.notes_outlined),
            onPressed: () {},
          )
        ],
        title: Text(
          "Pipe Grinder",
          style: TextStyle(color: Colors.white),
        ),
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
            StreamBuilder<BluetoothDeviceState>(
              stream: ble.state,
              initialData: BluetoothDeviceState.connecting,
              builder: (c, snapshot) => ListTile(
                leading: (snapshot.data == BluetoothDeviceState.connected)
                    ? Icon(Icons.bluetooth_connected)
                    : Icon(Icons.bluetooth_disabled),
                title: Text(
                  '파이프 연마기와 연결되었습니다.',
                  style: TextStyle(fontWeight: FontWeight.bold),
                ),
                subtitle: Text('Bluetooth Device: ${ble.name}'),
                trailing: StreamBuilder<bool>(
                  stream: ble.isDiscoveringServices,
                  initialData: false,
                  builder: (c, snapshot) => IndexedStack(
                    index: snapshot.data! ? 1 : 0,
                    children: <Widget>[
                      IconButton(
                        icon: Icon(Icons.settings),
                        onPressed: () => ble.discoverServices(),
                      ),
                      IconButton(
                        icon: SizedBox(
                          child: CircularProgressIndicator(
                            valueColor: AlwaysStoppedAnimation(Colors.grey),
                          ),
                          width: 18.0,
                          height: 18.0,
                        ),
                        onPressed: null,
                      )
                    ],
                  ),
                ),
              ),
            ),
            StreamBuilder<List<BluetoothService>>(
              stream: ble.services,
              initialData: [],
              builder: (c, snapshot) {
                if (snapshot.connectionState == ConnectionState.waiting) {
                  return const CircularProgressIndicator();
                }
                // return Text(
                //   "${snapshot.data!}",
                //   style: const TextStyle(fontSize: 10, color: Colors.blue),
                // );
                return Column(
                  children: _buildServiceTiles(snapshot.data!),
                );
              },
            ),
          ],
        ),
      ),
    );
  }
}

List<Widget> _buildServiceTiles(List<BluetoothService> services) {
  return services
      .map(
        (s) => ServiceTile(
          service: s,
          characteristicTiles: s.characteristics
              .map(
                (c) => CharacteristicTile(
                  characteristic: c,
                  onReadPressed: () => c.read(),
                  //48 ~ 57 -> 0 ~ 9
                  onWriteEMS: () async {
                    await c.write([48], withoutResponse: true);
                    await c.read();
                  },
                  onWriteRun: () async {
                    await c.write([49], withoutResponse: true);
                    await c.read();
                  },
                  onWriteForward: () async {
                    await c.write([50], withoutResponse: true);
                    await c.read();
                  },
                  onWriteBackward: () async {
                    await c.write([51], withoutResponse: true);
                    await c.read();
                  },
                ),
              )
              .toList(),
        ),
      )
      .toList();
}

class ServiceTile extends StatelessWidget {
  final BluetoothService service;
  final List<CharacteristicTile> characteristicTiles;

  const ServiceTile(
      {Key? key, required this.service, required this.characteristicTiles})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    if (characteristicTiles.length > 0 && characteristicTiles.length < 3) {
      if (characteristicTiles.length == 2) {
        characteristicTiles.removeAt(0);
      }
      return ExpansionTile(
        title: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Text(
              'Menu',
              style: TextStyle(fontWeight: FontWeight.bold),
            ),
          ],
        ),
        children: characteristicTiles,
      );
    } else {
      return ListTile(
          title: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Text('작동을 원하시면 아래 '),
          Text(
            'Menu',
            style: TextStyle(fontWeight: FontWeight.bold),
          ),
          Text('를 눌러주세요.'),
        ],
      ));
    }
  }
}

class CharacteristicTile extends StatelessWidget {
  final BluetoothCharacteristic characteristic;
  final VoidCallback? onReadPressed;
  final VoidCallback? onWriteEMS;
  final VoidCallback? onWriteRun;
  final VoidCallback? onWriteForward;
  final VoidCallback? onWriteBackward;

  const CharacteristicTile({
    Key? key,
    required this.characteristic,
    this.onReadPressed,
    this.onWriteEMS,
    this.onWriteRun,
    this.onWriteForward,
    this.onWriteBackward,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<List<int>>(
      stream: characteristic.value,
      initialData: characteristic.lastValue,
      builder: (c, snapshot) {
        return Column(
          children: <Widget>[
            _progressBar(context),
            _lists("EMS", "Emergency Stop", context, onWriteEMS),
            _lists("RUN", "Pipe Grinder Run", context, onWriteRun),
            _lists("Forward", "Go Forward", context, onWriteForward),
            _lists("Backward", "Go Back", context, onWriteBackward)
          ],
        );
      },
    );
  }
}

Widget _lists(name, value, context, onWritePressed) {
  return ExpansionTile(
    title: ListTile(
        title: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Text(name),
          ],
        ),
        subtitle: Text(value.toString()),
        contentPadding: EdgeInsets.all(0.0)),
    trailing: Row(
      mainAxisSize: MainAxisSize.min,
      children: <Widget>[
        IconButton(
          icon: Icon(Icons.play_arrow_rounded,
              color: Theme.of(context).iconTheme.color?.withOpacity(0.5)),
          onPressed: onWritePressed,
        ),
      ],
    ),
    // children: descriptorTiles,
  );
}

Widget _progressBar(BuildContext context) {
  return Card(
    child: Column(
      children: [
        Container(
          width: MediaQuery.of(context).size.width * 0.8,
          child: StepProgressIndicator(
            totalSteps: 10,
            currentStep: 1,
            selectedColor: Colors.black,
            unselectedColor: Colors.black26,
          ),
        ),
        Center(
          child: Padding(
            padding: const EdgeInsets.only(top: 20.0, bottom: 20.0),
            child: Text(
              "예상 연마 남은 시간: 03분 23초",
              style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
            ),
          ),
        )
      ],
    ),
  );
}

void showToasts(String str) {
  Fluttertoast.showToast(
      msg: str,
      toastLength: Toast.LENGTH_SHORT,
      gravity: ToastGravity.BOTTOM,
      timeInSecForIosWeb: 1,
      backgroundColor: Colors.blue[300],
      textColor: Colors.white,
      fontSize: 16.0);
}
