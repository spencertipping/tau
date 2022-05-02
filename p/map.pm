package tau::map;

use Encode       qw/decode/;
use List::Util   qw/max/;
use Scalar::Util qw/openhandle/;

use constant news => {n => [ 0, -1],
                      e => [ 1,  0],
                      w => [-1,  0],
                      s => [ 0,  1]};


sub new
{
  my ($class, $f) = @_;
  my %m;
  my $y  = 0;
  my $mx = 0;

  open my $fh, "<", $f or die "open < $f: $!";

  while (defined(my $l = <$fh>))
  {
    chomp $l;
    $l = decode 'UTF-8', $l;
    $m{pack LL => $_, $y} = ord substr $l, $_, 1 for 0..length($l)-1;
    $mx = max $mx, length($l) - 1;
    ++$y;
  }

  $m{mx} = $mx;
  $m{my} = $y - 1;

  bless \%m, $class;
}


sub n      { scalar(keys %{+shift}) - 2 }
sub bounds { @{+shift}{qw/mx my/} }
sub at     { ${+shift}{pack LL => @_} }


sub str
{
  my $self = shift;
  my ($mx, $my) = $self->bounds;
  my @ls;
  for my $y (0..$my)
  {
    push @ls, join"", map chr, grep defined, map $self->at($_, $y), 0..$mx;
  }
  join"\n", @ls;
}


sub next
{
  my ($self, $x, $y, $news) = @_;
  my ($dx, $dy) = @{news->{$news}};
  my ($mx, $my) = $self->bounds;

  $x += $dx;
  $y += $dy;

  while ($x >= 0 && $x <= $mx && $y >= 0 && $y <= $my)
  {
    my $c = $self->at($x, $y);
    return ($x, $y, chr $c) if defined $c && $c != 32;
    $x += $dx;
    $y += $dy;
  }
}
