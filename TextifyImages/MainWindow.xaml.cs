using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TextifyImages
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private List<string> Paths;

        public ObservableCollection<string> FileNames;

        public MainWindow()
        {
            InitializeComponent();
            Paths = new List<string>();
            FileNames = new ObservableCollection<string>();
            imagesList.ItemsSource = FileNames;
        }

        private void ImportImagesClick(object sender, RoutedEventArgs e)
        {
            var dialog = new OpenFileDialog();
            dialog.Multiselect = true;

            if (!(dialog.ShowDialog() ?? false))
            {
                return;
            }

            Paths.Clear();
            Paths.AddRange(dialog.FileNames);

            FileNames.Clear();
            foreach (var path in Paths)
            {
                FileNames.Add(System.IO.Path.GetFileName(path));
            }
        }
    }
}
