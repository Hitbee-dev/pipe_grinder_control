import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:get/get.dart';
import 'package:permission_handler/permission_handler.dart';

import 'home.dart';

var ble;

class Bluetooth extends StatelessWidget {
  final title = 'BLE Scan';
  @override
  Widget build(BuildContext context) {
    return BluetoothScan(title: title);
  }
}

class BluetoothScan extends StatefulWidget {
  BluetoothScan({Key? key, required this.title, this.state}) : super(key: key);
  final String title;
  final BluetoothState? state;
  @override
  _BluetoothScan createState() => _BluetoothScan();
}

class _BluetoothScan extends State<BluetoothScan> {
  FlutterBluePlus flutterBlue = FlutterBluePlus.instance;
  List<ScanResult> scanResultList = [];
  bool _isScanning = false;

  @override
  initState() {
    super.initState();
    if (Platform.isAndroid) {
      _checkPermissions();
      // _permission();
    }
    initBle(); // 블루투스 초기화
    Future.delayed(const Duration(milliseconds: 100), () {
      scan(); // IOS에서 바로 scan을 호출하게 되면 API 호출 전에 스캔을 하기 때문에 약간의 딜레이를 줌
    });
  }

  void initBle() {
    // BLE 스캔 상태 얻기 위한 리스너
    flutterBlue.isScanning.listen((isScanning) {
      _isScanning = isScanning;
      if (mounted) {
        setState(() {});
      }
    });
  }

  /*
  스캔 시작/정지 함수
  */
  scan() async {
    if (!_isScanning) {
      // 스캔 중이 아니라면
      // 기존에 스캔된 리스트 삭제
      scanResultList.clear();
      // 스캔 시작, 제한 시간 4초
      flutterBlue.startScan(timeout: Duration(seconds: 4));
      // 스캔 결과 리스너
      flutterBlue.scanResults.listen((results) {
        print("results: ${results}");
        scanResultList = results;
        // UI 갱신
        if (mounted) setState(() {});
      });
    } else {
      // 스캔 중이라면 스캔 정지
      flutterBlue.stopScan();
    }
  }

  /*
   여기서부터는 장치별 출력용 함수들
  */
  /*  장치의 신호값 위젯  */
  Widget deviceSignal(ScanResult r) {
    return Text(r.rssi.toString());
  }

  /* 장치의 MAC 주소 위젯  */
  Widget deviceMacAddress(ScanResult r) {
    return Text(r.device.id.id);
  }

  /* 장치의 명 위젯  */
  Widget deviceName(ScanResult r) {
    String name = '';

    if (r.device.name.isNotEmpty) {
      // device.name에 값이 있다면
      name = r.device.name;
    } else if (r.advertisementData.localName.isNotEmpty) {
      // advertisementData.localName에 값이 있다면
      name = r.advertisementData.localName;
    } else {
      // 둘다 없다면 이름 알 수 없음...
      name = 'N/A';
    }
    return Text(name);
  }

  /* BLE 아이콘 위젯 */
  Widget leading(ScanResult r) {
    return CircleAvatar(
      child: Icon(
        Icons.bluetooth,
        color: Colors.blueAccent,
      ),
      backgroundColor: Colors.transparent,
    );
  }

  /* 장치 아이템을 탭 했을때 호출 되는 함수 */
  void onTap(ScanResult r) {
    ble = r.device;
    ble.connect();
    print("check: ${r.device.services}");
    showToasts("연결 완료");
    Get.offAllNamed("home");
  }

  /* 장치 아이템 위젯 */
  Widget listItem(ScanResult r) {
    return ListTile(
      onTap: () => onTap(r),
      leading: leading(r),
      title: deviceName(r),
      subtitle: deviceMacAddress(r),
      trailing: deviceSignal(r),
    );
  }

  Scaffold bluetoothOn() {
    return Scaffold(
      appBar: AppBar(
        iconTheme: IconThemeData(color: Colors.white),
        backgroundColor: Colors.black87, //appbar 투명색
        centerTitle: true,
        elevation: 2.0, // 그림자 농도 0
        title: Text(
          widget.title,
          style: TextStyle(color: Colors.white),
        ),
      ),
      body: RefreshIndicator(
        onRefresh: () => scan(),
        child: ListView.separated(
          itemCount: scanResultList.length,
          itemBuilder: (context, index) {
            return listItem(scanResultList[index]);
          },
          separatorBuilder: (BuildContext context, int index) {
            return Divider();
          },
        ),
      ),

      /* 장치 검색 or 검색 중지  */
      floatingActionButton: FloatingActionButton(
        onPressed: scan,
        // 스캔 중이라면 stop 아이콘을, 정지상태라면 search 아이콘으로 표시
        child: Icon(_isScanning ? Icons.stop : Icons.search),
        backgroundColor: Colors.black87,
      ),
    );
  }

  Scaffold bluetoothOff(state) {
    return Scaffold(
      backgroundColor: Colors.lightBlue,
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Icon(
              Icons.bluetooth_disabled,
              size: 200.0,
              color: Colors.white54,
            ),
            Text(
              'Bluetooth Adapter is ${state != null ? state.toString().substring(15) : 'not available'}.',
            ),
          ],
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<BluetoothState>(
        stream: FlutterBluePlus.instance.state,
        initialData: BluetoothState.unknown,
        builder: (c, snapshot) {
          final state = snapshot.data;
          if (state == BluetoothState.on) {
            return bluetoothOn();
          }
          return bluetoothOff(state);
        });
  }
}

void _permission() async {
  var scan = await Permission.bluetoothScan.request();
  var advertise = await Permission.bluetoothAdvertise.request();
  var connect = await Permission.bluetoothConnect.request();
  if (scan.isGranted && advertise.isGranted && connect.isGranted) {}
}

Future<void> _checkPermissions() async {
  if (Platform.isAndroid) {
    Map<Permission, PermissionStatus> statuses = await [
      Permission.location,
      Permission.bluetoothScan,
      Permission.bluetoothConnect
    ].request();

    for (var status in statuses.entries) {
      if (status.key == Permission.location) {
        if (status.value.isGranted) {
          debugPrint('Location permission granted');
        } else {
          debugPrint("Location permission not granted");
        }
      } else if (status.key == Permission.bluetoothScan) {
        if (status.value.isGranted) {
          debugPrint('Bluetooth scan permission granted');
        } else {
          debugPrint('Bluetooth scan permission not granted');
        }
      } else if (status.key == Permission.bluetoothConnect) {
        if (status.value.isGranted) {
          debugPrint('Bluetooth connect permission granted');
        } else {
          debugPrint('Bluetooth connect permission not granted');
        }
      }
    }
  }
}
