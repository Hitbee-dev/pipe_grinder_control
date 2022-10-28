import 'package:flutter/material.dart';
import 'package:flutter/src/foundation/key.dart';
import 'package:flutter/src/widgets/framework.dart';
import 'package:get/get.dart';

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
        iconTheme: IconThemeData(color: Colors.black),
        backgroundColor: Colors.transparent, //appbar 투명색
        centerTitle: true,
        elevation: 0.0, // 그림자 농도 0
        leading: IconButton(
          icon: Icon(Icons.bluetooth),
          onPressed: () {
            print("블루투스");
            Get.toNamed("/bluetooth");
          },
        ),
        actions: [
          IconButton(
            icon: Icon(Icons.account_box_rounded),
            onPressed: () {},
          )
        ],
        title: Text(
          "메인 페이지",
          style: TextStyle(color: Colors.black),
        ),
      ),
    );
  }
}
