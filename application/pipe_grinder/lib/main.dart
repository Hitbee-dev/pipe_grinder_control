import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'home.dart';

void main() {
  runApp(const PipeGrinder());
}

class PipeGrinder extends StatelessWidget {
  const PipeGrinder({Key? key}) : super(key: key);

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return GetMaterialApp(
      title: 'PipeGrinder',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        primarySwatch: Colors.blueGrey,
      ),
      home: Home(),
      initialRoute: "/home",
      getPages: [
        getPage("/home", Home()),
      ],
    );
  }

  GetPage getPage(String name, Widget pageName) {
    return GetPage(
        name: name,
        page: () => pageName,
        transition: Transition.cupertinoDialog);
  }
}
