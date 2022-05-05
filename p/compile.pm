package tau::compile;


sub main
{
  my ($source) = @_;
  my $map = tau::map->new($source);

  my ($x, $y, @code) = tau::grammar::pstart->($map, 0, 0);
  print "$x $y @code\n";
}
